// Client_Prototype.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//
#include "stdafx.h"
#include "Client_Prototype.h"
#include "../Common/GeometryGenerator.h"
#include "Camera.h"
#include "Component_Manager.h"
#include "Scene.h"

#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")

#define MAX_LOADSTRING 100

UINT g_CbvSrvUavDescriptorSize;

// 전역 변수:

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:

int WINAPI wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	CComponent_Manager::GetInstance();
	CGameFramework_Client GameFramework(hInstance);
	
	try {
		if (!GameFramework.Initialize())
			return 0;

		//sever접속
		char temp_ip[15];
		char temp_id[15];
		cout << "Input id : ";
		cin >> temp_id;
		cout << "Input ip : ";
		cin >> temp_ip;
		GameFramework.m_pSocket = new CSocket(temp_id, temp_ip);
		if (GameFramework.m_pSocket->init()) {
			cs_packet_connect p_connect;
			strcpy_s(p_connect.id, GameFramework.m_pSocket->m_pid);
			p_connect.type = CS_LOGIN;
			p_connect.size = sizeof(cs_packet_connect);

			send(GameFramework.m_pSocket->clientSocket, (char*)&p_connect, sizeof(cs_packet_connect), 0);
		}
			
		return GameFramework.Run();
	}
	catch(DxException& e){
		HRESULT hr = GameFramework.GetDevice()->GetDeviceRemovedReason();
		int count = GameFramework.rendercount;
		MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
		return 0;
	}

	
	return 0;
}

CGameFramework_Client::CGameFramework_Client(HINSTANCE hInstance)
	:CD3DApp(hInstance)
{
}

CGameFramework_Client::~CGameFramework_Client()
{
	if (m_pScene) {
		m_pScene->ReleaseScene();
	}
	if (m_pSocket) m_pSocket->DataCleanup();

	CComponent_Manager::DestroyInstance();
}

//객체 초기화
bool CGameFramework_Client::Initialize()
{
	if (!CD3DApp::Initialize())
		return false;
	ThrowIfFailed(m_GraphicsCommandList->Reset(m_CommandAllocator.Get(), nullptr));

	m_pScene = new CScene(m_d3dDevice, m_GraphicsCommandList);
	if (!m_pScene->Initialize()) 
	{
		return false;
	}

	m_pSocket = nullptr;
	ThrowIfFailed(m_GraphicsCommandList->Close());

	ID3D12CommandList* cmdLists[] = { m_GraphicsCommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);
	FlushCommandQueue();

	return true;
}

void CGameFramework_Client::FlushCommandQueue(void)
{
}

void CGameFramework_Client::OnResize()
{
	CD3DApp::OnResize();
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProj, P);

	//m_CurrentCamera.Update(0);
	if (m_pScene)
		m_pScene->OnResize(AspectRatio());
}

void CGameFramework_Client::Update(const CTimer & gt)
{
	float fTimeElapsed = gt.DeltaTime();
	
	OnKeyboardInput(gt);
	//UpdateCamera(gt);
	
	// Cycle through the circular frame resource array.
	//mCurrFrameResourceIndex = (mCurrFrameResourceIndex + 1) % NUM_FRAME_RESOURCE;
	//mCurrFrameResource = mFrameResources[mCurrFrameResourceIndex].get();

	m_pScene->Update(gt, m_Fence.Get(), m_GraphicsCommandList.Get());

	//UpdateObjectCBs(gt);
	//UpdateMainPassCB(gt);
}

void CGameFramework_Client::Draw(const CTimer & gt)
{
	//auto cmdListAlloc = mCurrFrameResource->CmdListAlloc;

	// 커맨드리스트 리셋
	m_pScene->ResetCmdList(m_GraphicsCommandList.Get());
	//ThrowIfFailed(m_CommandAllocator->Reset());
	//ThrowIfFailed(m_GraphicsCommandList->Reset(m_CommandAllocator.Get(),NULL));

	// 뷰포트와 씨저렉트 설정. 이 작업은 반드시 커맨드리스트가 리셋된 후에  해야함.
	m_GraphicsCommandList->RSSetViewports(1, &m_ViewPort);
	m_GraphicsCommandList->RSSetScissorRects(1, &m_ScissorRect);

	//  리소스 사용에 대한 상태전이 지정
	m_GraphicsCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));


	// 백버퍼와 뎁스/스텐실버퍼 클리어
	m_GraphicsCommandList->ClearRenderTargetView(CurrentBackBufferView(), Colors::LightSteelBlue, 0, nullptr);
	m_GraphicsCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	// 렌더하기 위한 백버퍼를 지정
	m_GraphicsCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());
	
	// 씬 렌더링. 렌더코드는 여기에!
	/*ID3D12DescriptorHeap* descriptorHeaps[] = { mCbvHeap.Get() };
	m_GraphicsCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	m_GraphicsCommandList->SetGraphicsRootSignature(mRootSignature.Get());

	int passCbvIndex = mPassCbvOffset + mCurrFrameResourceIndex;
	auto passCbvHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(mCbvHeap->GetGPUDescriptorHandleForHeapStart());
	passCbvHandle.Offset(passCbvIndex, m_CbvSrvUavDescriptorSize);
	m_GraphicsCommandList->SetGraphicsRootDescriptorTable(1, passCbvHandle);*/

	m_pScene->Render(m_GraphicsCommandList.Get(), ++m_nFenceValue);

	// 리소스 사용에 대한 상태전이 지정
	m_GraphicsCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	// 명령리스트 작성 끝.
	ThrowIfFailed(m_GraphicsCommandList->Close());

	// 커맨드큐에 내가 내린 명령리스트를 집어넣고 실행
	ID3D12CommandList* cmdsLists[] = { m_GraphicsCommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	
	rendercount++;
	// 백/프론트 버퍼 교체
	ThrowIfFailed(m_dxgiSwapChain->Present(0, 0));
	m_CurrentBackBuffer = (m_CurrentBackBuffer + 1) % m_iSwapChainBufferCount;


	//mCurrFrameResource->Fence = ++m_nFenceValue;

	// Add an instruction to the command queue to set a new fence point. 
   // Because we are on the GPU timeline, the new fence point won't be 
   // set until the GPU finishes processing all the commands prior to this Signal().
	m_CommandQueue->Signal(m_Fence.Get(), m_nFenceValue);

}

void CGameFramework_Client::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	SetCapture(m_hMainWnd);
}

void CGameFramework_Client::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void CGameFramework_Client::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));

		// Update angles based on input to orbit camera around box.
		mTheta += dx;
		mPhi += dy;

		// Restrict the angle mPhi.
		mPhi = MathHelper::Clamp(mPhi, 0.1f, MathHelper::Pi - 0.1f);

		m_CurrentCamera.SetTheta(mTheta);
		m_CurrentCamera.SetPhi(mPhi);

	}
	else if ((btnState & MK_RBUTTON) != 0)
	{
		// Make each pixel correspond to 0.2 unit in the scene.
		float dx = 0.05f*static_cast<float>(x - mLastMousePos.x);
		float dy = 0.05f*static_cast<float>(y - mLastMousePos.y);

		// Update the camera radius based on input.
		mRadius += dx - dy;

		// Restrict the radius.
		mRadius = MathHelper::Clamp(mRadius, 5.0f, 150.0f);

		m_CurrentCamera.SetRadius(mRadius);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

//임시적으로 obj_type send
void CGameFramework_Client::OnKeyboardInput(const CTimer & gt)
{
	if (m_pScene)
		m_pScene->OnKeyboardInput(gt.DeltaTime());

	if (GetAsyncKeyState('1') & 0x8000)
		mIsWireframe = true;
	else
		mIsWireframe = false;


	/*float fSpeed = 3.f;
	if (GetAsyncKeyState('A') & 0x8000)
	{
		RenderItem* pRenderItem = (RenderItem*)m_OpaqueObjects[0]->Get_Component(L"RenderItem");
		
		pRenderItem->MovePos(&XMFLOAT3(-fSpeed * gt.DeltaTime(), 0.f, 0.f));
		pRenderItem->NumFramesDirty = NUM_FRAME_RESOURCE;
	}
	else if (GetAsyncKeyState('D') & 0x8000)
	{
		RenderItem* pRenderItem = (RenderItem*)m_OpaqueObjects[0]->Get_Component(L"RenderItem");

		pRenderItem->MovePos(&XMFLOAT3(fSpeed * gt.DeltaTime(), 0.f, 0.f));
		pRenderItem->NumFramesDirty = NUM_FRAME_RESOURCE;
	}

	if (GetAsyncKeyState('W') & 0x8000)
	{
		RenderItem* pRenderItem = (RenderItem*)m_OpaqueObjects[0]->Get_Component(L"RenderItem");

		pRenderItem->MovePos(&XMFLOAT3(0.f, 0.f, fSpeed * gt.DeltaTime()));
		pRenderItem->NumFramesDirty = NUM_FRAME_RESOURCE;
	}
	else if (GetAsyncKeyState('S') & 0x8000)
	{
		RenderItem* pRenderItem = (RenderItem*)m_OpaqueObjects[0]->Get_Component(L"RenderItem");

		pRenderItem->MovePos(&XMFLOAT3(0.f, 0.f, -fSpeed * gt.DeltaTime()));
		pRenderItem->NumFramesDirty = NUM_FRAME_RESOURCE;
	}*/
}


void CGameFramework_Client::BuildShapeGeometry()
{
	GeometryGenerator geoGen;
	GeometryGenerator::MeshData box = geoGen.CreateBox(1.0f, 1.0f, 1.0f, 3);
	GeometryGenerator::MeshData grid = geoGen.CreateGrid(1.0f, 1.0f, 60, 40);
	GeometryGenerator::MeshData sphere = geoGen.CreateSphere(0.5f, 20, 20);
	GeometryGenerator::MeshData cylinder = geoGen.CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20);

	//
	// We are concatenating all the geometry into one big vertex/index buffer.  So
	// define the regions in the buffer each submesh covers.
	//

	// Cache the vertex offsets to each object in the concatenated vertex buffer.
	UINT boxVertexOffset = 0;
	UINT gridVertexOffset = (UINT)box.Vertices.size();
	UINT sphereVertexOffset = gridVertexOffset + (UINT)grid.Vertices.size();
	UINT cylinderVertexOffset = sphereVertexOffset + (UINT)sphere.Vertices.size();

	// Cache the starting index for each object in the concatenated index buffer.
	UINT boxIndexOffset = 0;
	UINT gridIndexOffset = (UINT)box.Indices32.size();
	UINT sphereIndexOffset = gridIndexOffset + (UINT)grid.Indices32.size();
	UINT cylinderIndexOffset = sphereIndexOffset + (UINT)sphere.Indices32.size();

	// Define the SubmeshGeometry that cover different 
	// regions of the vertex/index buffers.

	SubmeshGeometry boxSubmesh;
	boxSubmesh.IndexCount = (UINT)box.Indices32.size();
	boxSubmesh.StartIndexLocation = boxIndexOffset;
	boxSubmesh.BaseVertexLocation = boxVertexOffset;

	SubmeshGeometry gridSubmesh;
	gridSubmesh.IndexCount = (UINT)grid.Indices32.size();
	gridSubmesh.StartIndexLocation = gridIndexOffset;
	gridSubmesh.BaseVertexLocation = gridVertexOffset;

	SubmeshGeometry sphereSubmesh;
	sphereSubmesh.IndexCount = (UINT)sphere.Indices32.size();
	sphereSubmesh.StartIndexLocation = sphereIndexOffset;
	sphereSubmesh.BaseVertexLocation = sphereVertexOffset;

	SubmeshGeometry cylinderSubmesh;
	cylinderSubmesh.IndexCount = (UINT)cylinder.Indices32.size();
	cylinderSubmesh.StartIndexLocation = cylinderIndexOffset;
	cylinderSubmesh.BaseVertexLocation = cylinderVertexOffset;

	//
	// Extract the vertex elements we are interested in and pack the
	// vertices of all the meshes into one vertex buffer.
	//

	auto totalVertexCount =
		box.Vertices.size() +
		grid.Vertices.size() +
		sphere.Vertices.size() +
		cylinder.Vertices.size();

	std::vector<Vertex> vertices(totalVertexCount);

	UINT k = 0;
	for (size_t i = 0; i < box.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = box.Vertices[i].Position;
		vertices[k].Color = XMFLOAT4(DirectX::Colors::DarkGreen);
	}

	for (size_t i = 0; i < grid.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = grid.Vertices[i].Position;
		vertices[k].Color = XMFLOAT4(DirectX::Colors::ForestGreen);
	}

	for (size_t i = 0; i < sphere.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = sphere.Vertices[i].Position;
		vertices[k].Color = XMFLOAT4(DirectX::Colors::Crimson);
	}

	for (size_t i = 0; i < cylinder.Vertices.size(); ++i, ++k)
	{
		vertices[k].Pos = cylinder.Vertices[i].Position;
		vertices[k].Color = XMFLOAT4(DirectX::Colors::SteelBlue);
	}

	std::vector<std::uint16_t> indices;
	indices.insert(indices.end(), std::begin(box.GetIndices16()), std::end(box.GetIndices16()));
	indices.insert(indices.end(), std::begin(grid.GetIndices16()), std::end(grid.GetIndices16()));
	indices.insert(indices.end(), std::begin(sphere.GetIndices16()), std::end(sphere.GetIndices16()));
	indices.insert(indices.end(), std::begin(cylinder.GetIndices16()), std::end(cylinder.GetIndices16()));

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	auto geo = std::make_unique<MeshGeometry>();
	geo->Name = "shapeGeo";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU));
	CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU));
	CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	geo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(m_d3dDevice.Get(),
		m_GraphicsCommandList.Get(), vertices.data(), vbByteSize, geo->VertexBufferUploader);

	geo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(m_d3dDevice.Get(),
		m_GraphicsCommandList.Get(), indices.data(), ibByteSize, geo->IndexBufferUploader);

	geo->VertexByteStride = sizeof(Vertex);
	geo->VertexBufferByteSize = vbByteSize;
	geo->IndexFormat = DXGI_FORMAT_R16_UINT;
	geo->IndexBufferByteSize = ibByteSize;

	geo->DrawArgs["box"] = boxSubmesh;
	geo->DrawArgs["grid"] = gridSubmesh;
	geo->DrawArgs["sphere"] = sphereSubmesh;
	geo->DrawArgs["cylinder"] = cylinderSubmesh;

	mGeometries[geo->Name] = std::move(geo);
}

void CGameFramework_Client::BuildPSOs()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC opaquePsoDesc;

	//
	// PSO for opaque objects.
	//
	ZeroMemory(&opaquePsoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	opaquePsoDesc.InputLayout = { mInputLayout.data(), (UINT)mInputLayout.size() };
	opaquePsoDesc.pRootSignature = mRootSignature.Get();
	opaquePsoDesc.VS =
	{
		reinterpret_cast<BYTE*>(mShaders["standardVS"]->GetBufferPointer()),
		mShaders["standardVS"]->GetBufferSize()
	};
	opaquePsoDesc.PS =
	{
		reinterpret_cast<BYTE*>(mShaders["opaquePS"]->GetBufferPointer()),
		mShaders["opaquePS"]->GetBufferSize()
	};
	opaquePsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	opaquePsoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	opaquePsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	opaquePsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	opaquePsoDesc.SampleMask = UINT_MAX;
	opaquePsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	opaquePsoDesc.NumRenderTargets = 1;
	opaquePsoDesc.RTVFormats[0] = m_BackBufferFormat;
	opaquePsoDesc.SampleDesc.Count = m_b4xMsaaState ? 4 : 1;
	opaquePsoDesc.SampleDesc.Quality = m_b4xMsaaState ? (m_n4xMsaaQuality - 1) : 0;
	opaquePsoDesc.DSVFormat = m_DepthStencilFormat;
	ThrowIfFailed(m_d3dDevice->CreateGraphicsPipelineState(&opaquePsoDesc, IID_PPV_ARGS(&mPSOs["opaque"])));


	//
	// PSO for opaque wireframe objects.
	//

	D3D12_GRAPHICS_PIPELINE_STATE_DESC opaqueWireframePsoDesc = opaquePsoDesc;
	opaqueWireframePsoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	ThrowIfFailed(m_d3dDevice->CreateGraphicsPipelineState(&opaqueWireframePsoDesc, IID_PPV_ARGS(&mPSOs["opaque_wireframe"])));
}

void CGameFramework_Client::BuildRenderItems()
{
	auto boxCItem = std::make_unique<CGameObject>();
	boxCItem->Initialize();

	RenderItem* boxRitem = (RenderItem*)boxCItem->Get_Component(L"RenderItem");
	XMStoreFloat4x4(&boxRitem->World, XMMatrixScaling(0.5f, 1.0f, 0.5f)*XMMatrixTranslation(0.0f, 0.5f, 0.0f));
	boxRitem->m_Transform.Set_World(&boxRitem->World);

	boxRitem->ObjCBIndex = 0;
	boxRitem->Geo = mGeometries["shapeGeo"].get();
	boxRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	boxRitem->IndexCount = boxRitem->Geo->DrawArgs["box"].IndexCount;
	boxRitem->StartIndexLocation = boxRitem->Geo->DrawArgs["box"].StartIndexLocation;
	boxRitem->BaseVertexLocation = boxRitem->Geo->DrawArgs["box"].BaseVertexLocation;
	m_vObjects.push_back(std::move(boxCItem));

	///////////////////////////////////////////////
	auto gridCItem = make_unique<CGameObject>();
	gridCItem->Initialize();

	RenderItem* gridRitem = (RenderItem*)gridCItem->Get_Component(L"RenderItem");
    gridRitem->World = MathHelper::Identity4x4();
	XMStoreFloat4x4(&gridRitem->World, XMMatrixScaling(20.0f, 1.0f, 20.0f)*XMMatrixTranslation(0.0f, 0.0f, 0.0f));
	gridRitem->m_Transform.Set_World(&gridRitem->World);

	gridRitem->ObjCBIndex = 1;
	gridRitem->Geo = mGeometries["shapeGeo"].get();
	gridRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    gridRitem->IndexCount = gridRitem->Geo->DrawArgs["grid"].IndexCount;
    gridRitem->StartIndexLocation = gridRitem->Geo->DrawArgs["grid"].StartIndexLocation;
    gridRitem->BaseVertexLocation = gridRitem->Geo->DrawArgs["grid"].BaseVertexLocation;
	m_vObjects.push_back(std::move(gridCItem));

	/*UINT objCBIndex = 2;
	for(int i = 0; i < 5; ++i)
	{
		auto leftCylRitem = std::make_unique<RenderItem>();
		auto rightCylRitem = std::make_unique<RenderItem>();
		auto leftSphereRitem = std::make_unique<RenderItem>();
		auto rightSphereRitem = std::make_unique<RenderItem>();

		XMMATRIX leftCylWorld = XMMatrixTranslation(-5.0f, 1.5f, -10.0f + i*5.0f);
		XMMATRIX rightCylWorld = XMMatrixTranslation(+5.0f, 1.5f, -10.0f + i*5.0f);

		XMMATRIX leftSphereWorld = XMMatrixTranslation(-5.0f, 3.5f, -10.0f + i*5.0f);
		XMMATRIX rightSphereWorld = XMMatrixTranslation(+5.0f, 3.5f, -10.0f + i*5.0f);

		XMStoreFloat4x4(&leftCylRitem->World, rightCylWorld);
		leftCylRitem->ObjCBIndex = objCBIndex++;
		leftCylRitem->Geo = mGeometries["shapeGeo"].get();
		leftCylRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		leftCylRitem->IndexCount = leftCylRitem->Geo->DrawArgs["cylinder"].IndexCount;
		leftCylRitem->StartIndexLocation = leftCylRitem->Geo->DrawArgs["cylinder"].StartIndexLocation;
		leftCylRitem->BaseVertexLocation = leftCylRitem->Geo->DrawArgs["cylinder"].BaseVertexLocation;

		XMStoreFloat4x4(&rightCylRitem->World, leftCylWorld);
		rightCylRitem->ObjCBIndex = objCBIndex++;
		rightCylRitem->Geo = mGeometries["shapeGeo"].get();
		rightCylRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		rightCylRitem->IndexCount = rightCylRitem->Geo->DrawArgs["cylinder"].IndexCount;
		rightCylRitem->StartIndexLocation = rightCylRitem->Geo->DrawArgs["cylinder"].StartIndexLocation;
		rightCylRitem->BaseVertexLocation = rightCylRitem->Geo->DrawArgs["cylinder"].BaseVertexLocation;

		XMStoreFloat4x4(&leftSphereRitem->World, leftSphereWorld);
		leftSphereRitem->ObjCBIndex = objCBIndex++;
		leftSphereRitem->Geo = mGeometries["shapeGeo"].get();
		leftSphereRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		leftSphereRitem->IndexCount = leftSphereRitem->Geo->DrawArgs["sphere"].IndexCount;
		leftSphereRitem->StartIndexLocation = leftSphereRitem->Geo->DrawArgs["sphere"].StartIndexLocation;
		leftSphereRitem->BaseVertexLocation = leftSphereRitem->Geo->DrawArgs["sphere"].BaseVertexLocation;

		XMStoreFloat4x4(&rightSphereRitem->World, rightSphereWorld);
		rightSphereRitem->ObjCBIndex = objCBIndex++;
		rightSphereRitem->Geo = mGeometries["shapeGeo"].get();
		rightSphereRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		rightSphereRitem->IndexCount = rightSphereRitem->Geo->DrawArgs["sphere"].IndexCount;
		rightSphereRitem->StartIndexLocation = rightSphereRitem->Geo->DrawArgs["sphere"].StartIndexLocation;
		rightSphereRitem->BaseVertexLocation = rightSphereRitem->Geo->DrawArgs["sphere"].BaseVertexLocation;

		mAllRitems.push_back(std::move(leftCylRitem));
		mAllRitems.push_back(std::move(rightCylRitem));
		mAllRitems.push_back(std::move(leftSphereRitem));
		mAllRitems.push_back(std::move(rightSphereRitem));
	}*/

	// All the render items are opaque.
	//for(auto& e : mAllRitems)
	//	mOpaqueRitems.push_back(e.get());

	for (auto& e : m_vObjects)
		m_OpaqueObjects.push_back(e.get());
		
}

void CGameFramework_Client::BuildComponent(void)
{
	
}

void CGameFramework_Client::BuildCamera(void)
{
	if (m_vObjects.empty())
		return;

	if (m_vObjects[0] == nullptr)
		return;

	if (m_vObjects[0]->Get_Component(L"RenderItem") == nullptr)
		return;

	m_CurrentCamera.SetTarget(m_vObjects[0].get());
}

void CGameFramework_Client::DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<CGameObject*>& ritems)
{
	
	//m_pScene->Render(cmdList);

	//cmdList->IASetVertexBuffers(0, 1, &ri->Geo->VertexBufferView());
	//cmdList->IASetIndexBuffer(&ri->Geo->IndexBufferView());
	//cmdList->IASetPrimitiveTopology(ri->PrimitiveType);

	//// Offset to the CBV in the descriptor heap for this object and for this frame resource.
	//UINT cbvIndex = mCurrFrameResourceIndex * (UINT)m_OpaqueObjects.size() + ri->ObjCBIndex;
	//auto cbvHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(mCbvHeap->GetGPUDescriptorHandleForHeapStart());
	//cbvHandle.Offset(cbvIndex, m_CbvSrvUavDescriptorSize);

	//cmdList->SetGraphicsRootDescriptorTable(0, cbvHandle);


	//cmdList->DrawIndexedInstanced(ri->IndexCount, 1, ri->StartIndexLocation, ri->BaseVertexLocation, 0);
}

void CGameFramework_Client::processPacket(char* buf)
{
	switch (buf[1])
	{
	case SC_CONNECTED:
	{
		sc_packet_login_ok* p_login = reinterpret_cast<sc_packet_login_ok*> (buf);
		m_client_id = p_login->id;
		//player객체 추가
	}
		break;
	case SC_ENTER:
	{
		sc_packet_enter* p_enter = reinterpret_cast<sc_packet_enter*>(buf);
		int other_id = p_enter->id;

		if (other_id == m_client_id) {
			//플레이어 캐릭터 초기위치 입력
		}
		else {
			//다른플레이어 초기 위치값 입력
		}
	}
		break;
	case SC_LEAVE: {
		sc_packet_leave* p_leave = reinterpret_cast<sc_packet_leave*>(buf);
		int other_id = p_leave->id;
		if (other_id == m_client_id) {
			//캐릭터 제거
		}
		else {
			//다른플레이어 제거
		}
	}
		break;
	case SC_MOVEMENT:
	{
		sc_packet_move* p_movement = reinterpret_cast<sc_packet_move*>(buf);
		int other_id = p_movement->id;
		if (other_id == m_client_id)
		{
			//플레이어 객체 이동
		}
		else
		{
			//다른플레이어 이동
		}
	}
		break;
	default:
		printf("Unknown PACKET type [%d]\n", buf[1]);
		break;
	}
}