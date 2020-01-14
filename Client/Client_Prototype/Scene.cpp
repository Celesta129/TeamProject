#include "stdafx.h"
#include "Scene.h"
#include "D3DApp.h"
#include "Box.h"
#include "Camera.h"
#include "Mesh.h"
#include "../Common/GeometryGenerator.h"

CScene::CScene(ComPtr<ID3D12Device> pDevice, ComPtr<ID3D12GraphicsCommandList> pCommandList)
	:m_d3dDevice(pDevice), m_GraphicsCommandList(pCommandList)
{
}


CScene::~CScene()
{
	if (m_pCamera)
		delete m_pCamera;
	if (m_pBoxMesh)
		delete m_pBoxMesh;
}

bool CScene::Initialize(UINT CbvSrvUavDescriptorSize)
{
	m_vObjects.reserve(MAX_OBJECT);

	BuildRootSignature();
	BuildShadersAndInputLayout();
	BuildShapeGeometry();
	BuildRenderItems();	// RenderItem == Object
	BuildFrameResources();
	BuildDescriptorHeaps();
	BuildConstantBufferViews(CbvSrvUavDescriptorSize);
	BuildPSOs();

	m_pBoxMesh = new CBoxMesh(m_d3dDevice.Get(), m_GraphicsCommandList.Get());

	m_pCamera = new CCamera;

	return true;
}

bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return false;
}

bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return false;
}

void CScene::BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
}

void CScene::ReleaseObjects()
{
	if (m_d3dGraphicsRootSignature) m_d3dGraphicsRootSignature->Release();
	
}

void CScene::AnimateObjects(float fTimeElapsed)
{
	
}

void CScene::Update(float fTimeElapsed, ID3D12Fence* pFence)
{
	m_CurrentFrameResourceIndex = (m_CurrentFrameResourceIndex + 1) % NumFrameResources;
	m_CurrentFrameResource = m_vFrameResources[m_CurrentFrameResourceIndex].get();

	//gpu�� ���� ������ �ڿ��� ����� �� ó���ߴ��� Ȯ��.
	// ���� �� ó�������ʾ����� gpu�� �� �潺�������� ��ɵ��� ó���Ҷ����� ��ٸ���.
	// �ᱹ ���� ��İ� �ٺ������� �ٸ��� ������ ��� ó���� �ڿ��� 3���� ����� �� GPU�۾��� �Ϸ�ɋ����� CPU�� �����ʰ� �ٸ� ��� ������ �Ѵٴ� ���������ִ�.
	if (m_CurrentFrameResource->Fence != 0 && pFence->GetCompletedValue() < m_CurrentFrameResource->Fence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
		ThrowIfFailed(pFence->SetEventOnCompletion(m_CurrentFrameResource->Fence, eventHandle));
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}


	// Update MainPassCB (�ַ� ī�޶� �� �� �Ⱥ��ϴ°�)
	if (m_pCamera)
		m_pCamera->Update(fTimeElapsed);

	// Update ObjectCB
	auto CurrentObjectCB = m_CurrentFrameResource->ObjectCB.get();
	for (auto obj : m_vObjects)
	{
		// ����ȭ �ʿ������ �׳� ������ ������� �����ϸ��.
		// ������Ʈ�� world �� ����� Dirty�� gNumFrameResources�� �Ǿ���Ѵ�. �׷��� �ٵ�.
		// ������Ʈ�� ������ ������۸� �����ִٸ� ������Ʈ�� �˾Ƽ� �������ٵ�..

		// ����� 3���� ������ ���ҽ��� ��ü�� �������*3�� �����Ѵ�.
		obj->Update(fTimeElapsed);
		if (obj->GetRenderItem()->NumFramesDirty > 0)
		{
			XMMATRIX world = XMLoadFloat4x4(obj->GetWorld());

			ObjectConstants objConstants;
			XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(world));

			CurrentObjectCB->CopyData(obj->GetRenderItem()->ObjCBIndex, objConstants);

			obj->GetRenderItem()->NumFramesDirty--;
		}
	}


}

void CScene::Render(UINT CbvSrvUavDescriptorSize)
{
	ID3D12DescriptorHeap* descriptorHeaps[] = { m_CbvHeap.Get() };
	m_GraphicsCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	m_GraphicsCommandList->SetGraphicsRootSignature(m_d3dGraphicsRootSignature.Get());

	int passCbvIndex = m_PassCbvOffset + m_CurrentFrameResourceIndex;
	auto passCbvHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(
		m_CbvHeap->GetGPUDescriptorHandleForHeapStart());
	passCbvHandle.Offset(passCbvIndex, CbvSrvUavDescriptorSize);
	// pass�� ������ 0���̴�.
	m_GraphicsCommandList->SetGraphicsRootDescriptorTable(0, passCbvHandle);

	auto objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));

	auto objectCB = m_CurrentFrameResource->ObjectCB->Resource();
	for (auto& obj : m_vObjects)
	{
		if (obj == nullptr)
			continue;

		UINT cbvIndex = m_CurrentFrameResourceIndex * (UINT)m_vObjects.size() + obj->GetRenderItem()->ObjCBIndex;
		auto cbvHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(
			m_CbvHeap->GetGPUDescriptorHandleForHeapStart());
		cbvHandle.Offset(cbvIndex, CbvSrvUavDescriptorSize);

		obj->Render(m_GraphicsCommandList.Get());

		// Object ������ 1��
		m_GraphicsCommandList->SetGraphicsRootDescriptorTable(1, cbvHandle);

		m_GraphicsCommandList->DrawIndexedInstanced(obj->GetRenderItem()->m_IndexCount, 1,
			obj->GetRenderItem()->StartIndexLocation, obj->GetRenderItem()->BaseVertexLocation,0);
	}


}

void CScene::ReleaseUploadBuffers()
{
}

void CScene::OnResize(float fAspectRatio)
{
	if (m_pCamera)
		m_pCamera->GenerateProjectionMatrix(1.0f, 1000.f, fAspectRatio, 0.25 * MathHelper::Pi);

}


void CScene::BuildDescriptorHeaps()
{
	UINT objCount = (UINT)m_vObjects.capacity();
	// �����Ӹ��ҽ��� ��ü���� �ϳ��� CBV �����ڰ� �ʿ�.
	// +1�� �� ������ �ڿ��� �ʿ��� �н��� CBV�� ���� ����

	UINT numDescriptor = (objCount + 1) * gNumFrameResources;
	m_PassCbvOffset = objCount * gNumFrameResources;			// ������ ������ gNumFrame��ŭ ������ �����. PassCB�� ���Ѱ���.

	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
	cbvHeapDesc.NumDescriptors = numDescriptor;		// 0��  : �н�, 1�� : ������Ʈ
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.NodeMask = 0;

	ThrowIfFailed(m_d3dDevice->CreateDescriptorHeap(&cbvHeapDesc,
		IID_PPV_ARGS(&m_CbvHeap)));
}

void CScene::BuildConstantBufferViews(UINT CbvSrvUavDescriptorSize)
{
	UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));
	UINT objCount = (UINT)m_vObjects.capacity();

	// ������Ʈ�� �������� �߰�/�����ɶ��� ����ؾ���. ������Ʈ���� ������۸� ������ ����?
	// ������ �ڿ��� ������Ʈ�� �����ְ� ������Ʈ�� ������۸� �����ִٸ�?

	// �� ������ �ڿ�����
	for (UINT FrameIndex = 0; FrameIndex < gNumFrameResources; ++FrameIndex)
	{
		// �� ������Ʈ���� ������۸� �����.
		auto ObjectCB = m_vFrameResources[FrameIndex]->ObjectCB->Resource();
		for (UINT i = 0; i < objCount; ++i)
		{
			D3D12_GPU_VIRTUAL_ADDRESS cbAddress = ObjectCB->GetGPUVirtualAddress();

			// ������ۿ��� i��° ��ü�� ��������� ������
			cbAddress += i * objCBByteSize;

			// ������ ������ i��° ��ü�� CBV�� ������
			int heapIndex = FrameIndex * objCount + i;
			auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_CbvHeap->GetCPUDescriptorHandleForHeapStart());
			handle.Offset(heapIndex, CbvSrvUavDescriptorSize);

			D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
			cbvDesc.BufferLocation = cbAddress;
			cbvDesc.SizeInBytes = objCBByteSize;

			m_d3dDevice->CreateConstantBufferView(&cbvDesc, handle);
		}
	}

}

void CScene::BuildRootSignature(void)
{
	CD3DX12_DESCRIPTOR_RANGE cbvTable0;
	cbvTable0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);

	CD3DX12_DESCRIPTOR_RANGE cbvTable1;
	cbvTable1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);


	// ��Ʈ �Ű������� ������ ���̺��̰ų� ��Ʈ ������ �Ǵ� ��Ʈ ����̴�
	CD3DX12_ROOT_PARAMETER slotRootParameter[2];

	// CBV �ϳ��� ��� ������ ���̺��� �ΰ� �����Ѵ�.
	slotRootParameter[0].InitAsDescriptorTable(1, &cbvTable0);
	slotRootParameter[1].InitAsDescriptorTable(1, &cbvTable1);


	// ��Ʈ������ ��Ʈ �Ű��������� �迭�̴�
	int numCBV = 2;
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(numCBV, slotRootParameter, 0, nullptr,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	// ������� �ϳ��� ������ ������ ������ ����Ű��
	// ���� �ϳ��� �̷���� ��Ʈ ������ �����Ѵ�.
	ComPtr<ID3DBlob> serializedRootSig = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
	{
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}
	ThrowIfFailed(hr);

	ThrowIfFailed(m_d3dDevice->CreateRootSignature(
		0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(&m_d3dGraphicsRootSignature)));
}

void CScene::BuildShadersAndInputLayout(void)
{
	HRESULT hr = S_OK;

	m_vsByteCode = d3dUtil::CompileShader(L"Shaders\\color.hlsl", nullptr, "VS", "vs_5_1");
	m_psByteCode = d3dUtil::CompileShader(L"Shaders\\color.hlsl", nullptr, "PS", "ps_5_1");

	m_InputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
}


void CScene::BuildPSOs(void)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	psoDesc.InputLayout = { m_InputLayout.data(), (UINT)m_InputLayout.size() };
	psoDesc.pRootSignature = m_d3dGraphicsRootSignature.Get();
	psoDesc.VS =
	{
		reinterpret_cast<BYTE*>(m_vsByteCode->GetBufferPointer()),
		m_vsByteCode->GetBufferSize()
	};
	psoDesc.PS =
	{
		reinterpret_cast<BYTE*>(m_psByteCode->GetBufferPointer()),
		m_psByteCode->GetBufferSize()
	};
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = CD3DApp::GetInstance()->GetBackBufferFormat();
	psoDesc.SampleDesc.Count = CD3DApp::GetInstance()->Get4xMsaaState() ? 4 : 1;
	psoDesc.SampleDesc.Quality = CD3DApp::GetInstance()->Get4xMsaaState() ? (CD3DApp::GetInstance()->Get4xMsaaQuality() - 1) : 0;
	psoDesc.DSVFormat = CD3DApp::GetInstance()->GetDepthStencilFormat();
	ThrowIfFailed(m_d3dDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_PSO)));
}

void CScene::BuildFrameResources(void)
{
	for (int i = 0; i < gNumFrameResources; ++i)
	{
		m_vFrameResources.push_back(make_unique<FrameResource>(m_d3dDevice.Get(), 1, MAX_OBJECT));
	}
}

void CScene::BuildShapeGeometry(void)
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

void CScene::BuildRenderItems(void)
{
	auto boxRitem = new CObject;
	XMStoreFloat4x4(boxRitem->GetWorld(), XMMatrixScaling(0.5f, 1.0f, 0.5f)*XMMatrixTranslation(0.0f, 0.5f, 0.0f));
	boxRitem->GetRenderItem()->ObjCBIndex = 0;
	boxRitem->GetRenderItem()->Geo = mGeometries["shapeGeo"].get();
	boxRitem->GetRenderItem()->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	boxRitem->GetRenderItem()->m_IndexCount = boxRitem->GetRenderItem()->Geo->DrawArgs["box"].IndexCount;
	boxRitem->GetRenderItem()->StartIndexLocation = boxRitem->GetRenderItem()->Geo->DrawArgs["box"].StartIndexLocation;
	boxRitem->GetRenderItem()->BaseVertexLocation = boxRitem->GetRenderItem()->Geo->DrawArgs["box"].BaseVertexLocation;
	m_vObjects.push_back(boxRitem);

	//auto gridRitem = std::make_unique<RenderItem>();
	//gridRitem->World = MathHelper::Identity4x4();
	//XMStoreFloat4x4(&gridRitem->World, XMMatrixScaling(20.0f, 1.0f, 20.0f)*XMMatrixTranslation(0.0f, 0.0f, 0.0f));
	//gridRitem->ObjCBIndex = 1;
	//gridRitem->Geo = mGeometries["shapeGeo"].get();
	//gridRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	//gridRitem->IndexCount = gridRitem->Geo->DrawArgs["grid"].IndexCount;
	//gridRitem->StartIndexLocation = gridRitem->Geo->DrawArgs["grid"].StartIndexLocation;
	//gridRitem->BaseVertexLocation = gridRitem->Geo->DrawArgs["grid"].BaseVertexLocation;
	//mAllRitems.push_back(std::move(gridRitem));

	//UINT objCBIndex = 2;
	//for (int i = 0; i < 5; ++i)
	//{
	//	auto leftCylRitem = std::make_unique<RenderItem>();
	//	auto rightCylRitem = std::make_unique<RenderItem>();
	//	auto leftSphereRitem = std::make_unique<RenderItem>();
	//	auto rightSphereRitem = std::make_unique<RenderItem>();

	//	XMMATRIX leftCylWorld = XMMatrixTranslation(-5.0f, 1.5f, -10.0f + i * 5.0f);
	//	XMMATRIX rightCylWorld = XMMatrixTranslation(+5.0f, 1.5f, -10.0f + i * 5.0f);

	//	XMMATRIX leftSphereWorld = XMMatrixTranslation(-5.0f, 3.5f, -10.0f + i * 5.0f);
	//	XMMATRIX rightSphereWorld = XMMatrixTranslation(+5.0f, 3.5f, -10.0f + i * 5.0f);

	//	XMStoreFloat4x4(&leftCylRitem->World, rightCylWorld);
	//	leftCylRitem->ObjCBIndex = objCBIndex++;
	//	leftCylRitem->Geo = mGeometries["shapeGeo"].get();
	//	leftCylRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	//	leftCylRitem->IndexCount = leftCylRitem->Geo->DrawArgs["cylinder"].IndexCount;
	//	leftCylRitem->StartIndexLocation = leftCylRitem->Geo->DrawArgs["cylinder"].StartIndexLocation;
	//	leftCylRitem->BaseVertexLocation = leftCylRitem->Geo->DrawArgs["cylinder"].BaseVertexLocation;

	//	XMStoreFloat4x4(&rightCylRitem->World, leftCylWorld);
	//	rightCylRitem->ObjCBIndex = objCBIndex++;
	//	rightCylRitem->Geo = mGeometries["shapeGeo"].get();
	//	rightCylRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	//	rightCylRitem->IndexCount = rightCylRitem->Geo->DrawArgs["cylinder"].IndexCount;
	//	rightCylRitem->StartIndexLocation = rightCylRitem->Geo->DrawArgs["cylinder"].StartIndexLocation;
	//	rightCylRitem->BaseVertexLocation = rightCylRitem->Geo->DrawArgs["cylinder"].BaseVertexLocation;

	//	XMStoreFloat4x4(&leftSphereRitem->World, leftSphereWorld);
	//	leftSphereRitem->ObjCBIndex = objCBIndex++;
	//	leftSphereRitem->Geo = mGeometries["shapeGeo"].get();
	//	leftSphereRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	//	leftSphereRitem->IndexCount = leftSphereRitem->Geo->DrawArgs["sphere"].IndexCount;
	//	leftSphereRitem->StartIndexLocation = leftSphereRitem->Geo->DrawArgs["sphere"].StartIndexLocation;
	//	leftSphereRitem->BaseVertexLocation = leftSphereRitem->Geo->DrawArgs["sphere"].BaseVertexLocation;

	//	XMStoreFloat4x4(&rightSphereRitem->World, rightSphereWorld);
	//	rightSphereRitem->ObjCBIndex = objCBIndex++;
	//	rightSphereRitem->Geo = mGeometries["shapeGeo"].get();
	//	rightSphereRitem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	//	rightSphereRitem->IndexCount = rightSphereRitem->Geo->DrawArgs["sphere"].IndexCount;
	//	rightSphereRitem->StartIndexLocation = rightSphereRitem->Geo->DrawArgs["sphere"].StartIndexLocation;
	//	rightSphereRitem->BaseVertexLocation = rightSphereRitem->Geo->DrawArgs["sphere"].BaseVertexLocation;

	//	mAllRitems.push_back(std::move(leftCylRitem));
	//	mAllRitems.push_back(std::move(rightCylRitem));
	//	mAllRitems.push_back(std::move(leftSphereRitem));
	//	mAllRitems.push_back(std::move(rightSphereRitem));
	//}

	//// All the render items are opaque.
	//for (auto& e : mAllRitems)
	//	mOpaqueRitems.push_back(e.get());
}

void CScene::BuildMesh(void)
{
}

void CScene::BuildObject(void)
{
	
}
