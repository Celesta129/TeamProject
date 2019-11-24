#include "stdafx.h"
#include "Scene.h"
#include "D3DApp.h"
#include "Box.h"
#include "Camera.h"
#include "Mesh.h"

CScene::CScene(ComPtr<ID3D12Device> pDevice, ComPtr<ID3D12GraphicsCommandList> pCommandList)
	:m_d3dDevice(pDevice), m_GraphicsCommandList(pCommandList)
{
}


CScene::~CScene()
{
	if (m_pCamera)
		delete m_pCamera;
	if (m_pBox)
		delete m_pBox;
	if (m_pBoxMesh)
		delete m_pBoxMesh;
}

bool CScene::Initialize()
{
	BuildDescriptorHeaps();
	BuildConstantBuffers();
	BuildRootSignature();
	BuildShadersAndInputLayout();
	BuildBoxGeometry();
	BuildPSO();
	
	m_pBoxMesh = new CBoxMesh(m_d3dDevice.Get(), m_GraphicsCommandList.Get());
	m_pBox = new CBox;
	m_pBox->SetMesh(m_pBoxMesh);
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

void CScene::Update(float fTimeElapsed)
{
	//if (m_pCamera)
		//m_pCamera->Update(fTimeElapsed);

	if (m_pBox)
		m_pBox->Update(fTimeElapsed);

	float x = mRadius * sinf(mPhi)*cosf(mTheta);
	float z = mRadius * sinf(mPhi)*sinf(mTheta);
	float y = mRadius * cosf(mPhi);

	// Build the view matrix.
	XMVECTOR boxPos = m_pBox->GetPos();

	XMVECTOR pos = boxPos + XMVectorSet(x, y, z, 0.0f);
	XMVECTOR target = boxPos; m_pBox->GetPos();//XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	
	XMMATRIX scale = XMMatrixScaling(50.f, 100.f, 50.f);

	XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, view);
	
	XMMATRIX world = XMLoadFloat4x4(&mWorld);
	XMMATRIX proj = XMLoadFloat4x4(&mProj);
	
	XMMATRIX ViewProj = view * proj;
	XMMATRIX worldViewProj = scale * world * view*proj;

	// Update the constant buffer with the latest worldViewProj matrix.

	CameraConstants camConstants;
	XMStoreFloat4x4(&camConstants.ViewProj, XMMatrixTranspose(ViewProj));

	ObjectConstants objConstants;
	XMStoreFloat4x4(&objConstants.WorldViewProj, XMMatrixTranspose(worldViewProj));
	m_ObjectCB->CopyData(0, objConstants);


}

void CScene::Render()
{
	ID3D12DescriptorHeap* descriptorHeaps[] = { m_CbvHeap.Get() };
	m_GraphicsCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	m_GraphicsCommandList->SetGraphicsRootSignature(m_d3dGraphicsRootSignature.Get());

	m_GraphicsCommandList->IASetVertexBuffers(0, 1, &m_BoxGeo->VertexBufferView());
	m_GraphicsCommandList->IASetIndexBuffer(&m_BoxGeo->IndexBufferView());
	m_GraphicsCommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_GraphicsCommandList->SetGraphicsRootDescriptorTable(0, m_CbvHeap->GetGPUDescriptorHandleForHeapStart());

	m_GraphicsCommandList->DrawIndexedInstanced(
		m_BoxGeo->DrawArgs["box"].IndexCount,
		1, 0, 0, 0);

	m_pBox->Render(m_GraphicsCommandList.Get());
}

void CScene::ReleaseUploadBuffers()
{
}

void CScene::OnResize(float fAspectRatio)
{
	if (m_pCamera)
		m_pCamera->GenerateProjectionMatrix(1.0f, 1000.f, fAspectRatio, 0.25 * MathHelper::Pi);
	// The window resized, so update the aspect ratio and recompute the projection matrix.
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*MathHelper::Pi, fAspectRatio, 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProj, P);
}


void CScene::BuildDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
	cbvHeapDesc.NumDescriptors = 1;		// 0번  : 카메라, 1번 : 오브젝트
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.NodeMask = 0;

	ThrowIfFailed(m_d3dDevice->CreateDescriptorHeap(&cbvHeapDesc,
		IID_PPV_ARGS(&m_CbvHeap)));
}

void CScene::BuildConstantBuffers(void)
{
	/*m_CamCB = std::make_unique<UploadBuffer<CameraConstants>>(m_d3dDevice.Get(), 1, true);
	
	UINT camCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(CameraConstants));

	D3D12_GPU_VIRTUAL_ADDRESS cbAddress = m_CamCB->Resource()->GetGPUVirtualAddress();

	int camCBufIndex = 0;
	cbAddress += camCBufIndex * camCBByteSize;

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;

	cbvDesc.BufferLocation = cbAddress;
	cbvDesc.SizeInBytes = d3dUtil::CalcConstantBufferByteSize(sizeof(CameraConstants));
	m_d3dDevice->CreateConstantBufferView(
		&cbvDesc,
		m_CbvHeap->GetCPUDescriptorHandleForHeapStart());*/

	////////////////////////////////////////////////////////////////////////////////////////
	m_ObjectCB = std::make_unique<UploadBuffer<ObjectConstants>>(m_d3dDevice.Get(), 1, true);

	UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));

	D3D12_GPU_VIRTUAL_ADDRESS cbAddress = m_ObjectCB->Resource()->GetGPUVirtualAddress();
	// Offset to the ith object constant buffer in the buffer.
	int boxCBufIndex = 0;
	cbAddress += boxCBufIndex * objCBByteSize;

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = cbAddress;
	cbvDesc.SizeInBytes = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));

	m_d3dDevice->CreateConstantBufferView(
		&cbvDesc,
		m_CbvHeap->GetCPUDescriptorHandleForHeapStart());
}

void CScene::BuildRootSignature(void)
{
	// 루트 매개변수는 서술자 테이블이거나 루트 서술자 또는 루트 상수이다
	CD3DX12_ROOT_PARAMETER slotRootParameter[1];

	// CBV 하나를 담는 서술자 테이블을 생성한다.
	CD3DX12_DESCRIPTOR_RANGE cbvTable;
	cbvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
	slotRootParameter[0].InitAsDescriptorTable(1, &cbvTable);

	// 루트서명은 루트 매개변수들의 배열이다
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(1, slotRootParameter, 0, nullptr,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	// 상수버퍼 하나로 구성된 서술자 구간을 가리키는
	// 슬롯 하나로 이루어진 루트 서명을 생성한다.
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

void CScene::BuildBoxGeometry(void)
{
	std::array<Vertex, 8> vertices =
	{
		Vertex({ XMFLOAT3(-1.f, -1.f, -1.f), XMFLOAT4(Colors::White) }),
		Vertex({ XMFLOAT3(-1.f, +1.f, -1.f), XMFLOAT4(Colors::Black) }),
		Vertex({ XMFLOAT3(+1.f, +1.f, -1.f), XMFLOAT4(Colors::Red) }),
		Vertex({ XMFLOAT3(+1.f, -1.f, -1.f), XMFLOAT4(Colors::Green) }),
		Vertex({ XMFLOAT3(-1.f, -1.f, +1.f), XMFLOAT4(Colors::Blue) }),
		Vertex({ XMFLOAT3(-1.f, +1.f, +1.f), XMFLOAT4(Colors::Yellow) }),
		Vertex({ XMFLOAT3(+1.f, +1.f, +1.f), XMFLOAT4(Colors::Cyan) }),
		Vertex({ XMFLOAT3(+1.f, -1.f, +1.f), XMFLOAT4(Colors::Magenta) })
	};

	std::array<std::uint16_t, 36> indices =
	{
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3,
		4, 3, 7
	};

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	m_BoxGeo = std::make_unique<MeshGeometry>();
	m_BoxGeo->Name = "boxGeo";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &m_BoxGeo->VertexBufferCPU));
	CopyMemory(m_BoxGeo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &m_BoxGeo->IndexBufferCPU));
	CopyMemory(m_BoxGeo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	m_BoxGeo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(m_d3dDevice.Get(),
		m_GraphicsCommandList.Get(), vertices.data(), vbByteSize, m_BoxGeo->VertexBufferUploader);

	m_BoxGeo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(m_d3dDevice.Get(),
		m_GraphicsCommandList.Get(), indices.data(), ibByteSize, m_BoxGeo->IndexBufferUploader);

	m_BoxGeo->VertexByteStride = sizeof(Vertex);
	m_BoxGeo->VertexBufferByteSize = vbByteSize;
	m_BoxGeo->IndexFormat = DXGI_FORMAT_R16_UINT;
	m_BoxGeo->IndexBufferByteSize = ibByteSize;

	SubmeshGeometry submesh;
	submesh.IndexCount = (UINT)indices.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	m_BoxGeo->DrawArgs["box"] = submesh;
}

void CScene::BuildPSO(void)
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
