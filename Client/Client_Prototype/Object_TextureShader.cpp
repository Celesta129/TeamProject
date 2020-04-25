#include "Object_TextureShader.h"



CObject_TextureShader::CObject_TextureShader()
{
}


CObject_TextureShader::~CObject_TextureShader()
{
}

void CObject_TextureShader::Update(const CTimer & timer, ID3D12Fence * pFence, CCamera * pCamera)
{
}

D3D12_INPUT_LAYOUT_DESC CObject_TextureShader::CreateInputLayout()
{
	return D3D12_INPUT_LAYOUT_DESC();
}

D3D12_RASTERIZER_DESC CObject_TextureShader::CreateRasterizerState()
{
	return D3D12_RASTERIZER_DESC();
}

D3D12_BLEND_DESC CObject_TextureShader::CreateBlendState()
{
	return D3D12_BLEND_DESC();
}

D3D12_DEPTH_STENCIL_DESC CObject_TextureShader::CreateDepthStencilState()
{
	return D3D12_DEPTH_STENCIL_DESC();
}

void CObject_TextureShader::CreatePSO(ID3D12Device * pd3dDevice, UINT nRenderTargets, int index)
{
}

void CObject_TextureShader::CreateFrameResources(ID3D12Device * pd3dDevice)
{
	for (int i = 0; i < NUM_FRAME_RESOURCE; ++i)
	{
		m_vFrameResources.push_back(std::make_unique<FrameResource>(pd3dDevice,
			1, (UINT)m_vpObjects.size()));
	}
}

void CObject_TextureShader::CreateDescriptorHeaps(ID3D12Device * pd3dDevice)
{
	UINT objCount = (UINT)m_vpObjects.size();
	UINT matCount = (UINT)m_vMaterial.size();

	// Need a CBV descriptor for each object for each frame resource,
	// +1 for the perPass CBV for each frame resource.
	UINT numDescriptors = (objCount + matCount +1) * NUM_FRAME_RESOURCE;

	// Save an offset to the start of the pass CBVs.  These are the last 3 descriptors.
	// 마지막 3개의 디스크립터(서술자)는 패스별 CBV를 위한것이다.
	mPassCbvOffset = (objCount + matCount) * NUM_FRAME_RESOURCE;
	m_iMaterialCbvOffset = objCount * NUM_FRAME_RESOURCE;

	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
	cbvHeapDesc.NumDescriptors = numDescriptors;
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.NodeMask = 0;
	ThrowIfFailed(pd3dDevice->CreateDescriptorHeap(&cbvHeapDesc,
		IID_PPV_ARGS(&m_CbvSrvDescriptorHeap)));

	
	////
	//// Fill out the heap with actual descriptors.
	////
	//CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(m_CbvSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	//auto grassTex = mTextures["grassTex"]->Resource;
	//auto waterTex = mTextures["waterTex"]->Resource;
	//auto fenceTex = mTextures["fenceTex"]->Resource;

	//D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	//srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	//srvDesc.Format = grassTex->GetDesc().Format;
	//srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	//srvDesc.Texture2D.MostDetailedMip = 0;
	//srvDesc.Texture2D.MipLevels = -1;
	//pd3dDevice->CreateShaderResourceView(grassTex.Get(), &srvDesc, hDescriptor);

	//
	//// next descriptor
	//hDescriptor.Offset(1, g_CbvSrvUavDescriptorSize);

	//srvDesc.Format = waterTex->GetDesc().Format;
	//pd3dDevice->CreateShaderResourceView(waterTex.Get(), &srvDesc, hDescriptor);

	//// next descriptor
	//hDescriptor.Offset(1, g_CbvSrvUavDescriptorSize);

	//srvDesc.Format = fenceTex->GetDesc().Format;
	//pd3dDevice->CreateShaderResourceView(fenceTex.Get(), &srvDesc, hDescriptor);
}

void CObject_TextureShader::CreateRootSignature(ID3D12Device * pd3dDevice)
{
	UINT numSlot = 4;

	CD3DX12_DESCRIPTOR_RANGE descTable[4];
	descTable[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, CBV_REGISTER_PASS);		// for PassCB :register(b0)
	descTable[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, CBV_REGISTER_OBJECT);		// for ObjectCB : :register(b1)
	descTable[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, CBV_REGISTER_MATERIAL);	// for MaterialCB : register(b2)
	descTable[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, SRV_REGISTER_TEXTURE);	// for Texture : register (t0)


	// Root parameter can be a table, root descriptor or root constants.
	CD3DX12_ROOT_PARAMETER slotRootParameter[4];

	// Perfomance TIP: Order from most frequent to least frequent.
	slotRootParameter[RPI_PassConstant].InitAsDescriptorTable(1, &descTable[0]); // register b0 : passCB			// same as 
	slotRootParameter[RPI_ObjectConstant].InitAsDescriptorTable(1, &descTable[1]); // register b1 : ObjectCB
	slotRootParameter[RPI_MaterialConstant].InitAsDescriptorTable(1, &descTable[2]);; // register b2 : Material
	slotRootParameter[RPI_Texture].InitAsDescriptorTable(1, &descTable[3],D3D12_SHADER_VISIBILITY_PIXEL);			// register t0 : Texture

	// auto staticSamplers = GetStaticSamplers();	// skip

	// A root signature is an array of root parameters.
	
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(numSlot, slotRootParameter,
		0, nullptr,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	// create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
	ComPtr<ID3DBlob> serializedRootSig = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
	{
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}
	ThrowIfFailed(hr);

	ThrowIfFailed(pd3dDevice->CreateRootSignature(
		0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(m_RootSignature[PSO_OBJECT].GetAddressOf())));
}

void CObject_TextureShader::CreateShader(ID3D12Device * pd3dDevice, ID3D12RootSignature * pd3dGraphicsRootSignature)
{
}

void CObject_TextureShader::CreateShaderVariables(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{

}

void CObject_TextureShader::CreateConstantBufferViews(ID3D12Device * pDevice)
{
	UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));

	UINT objCount = (UINT)m_vpObjects.size();

	// Need a CBV descriptor for each object for each frame resource.
	for (int frameIndex = 0; frameIndex < NUM_FRAME_RESOURCE; ++frameIndex)
	{
		auto objectCB = m_vFrameResources[frameIndex]->ObjectCB->Resource();
		for (UINT i = 0; i < objCount; ++i)
		{
			D3D12_GPU_VIRTUAL_ADDRESS cbAddress = objectCB->GetGPUVirtualAddress();

			// Offset to the ith object constant buffer in the buffer.
			cbAddress += i * objCBByteSize;

			// Offset to the object cbv in the descriptor heap.
			int heapIndex = frameIndex * objCount + i;
			auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_CbvSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
			handle.Offset(heapIndex, g_CbvSrvUavDescriptorSize);

			D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
			cbvDesc.BufferLocation = cbAddress;
			cbvDesc.SizeInBytes = objCBByteSize;	// 256바이트의 정수배로 정렬되어야한다.

			pDevice->CreateConstantBufferView(&cbvDesc, handle);
		}
	}

	UINT materialCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(MaterialConstants));
	UINT materialCount = (UINT)m_vMaterial.size();

	for (int frameIndex = 0; frameIndex < NUM_FRAME_RESOURCE; ++frameIndex)
	{
		auto materialCB = m_vFrameResources[frameIndex]->MaterialCB->Resource();
		for (UINT i = 0; i < materialCount; ++i)
		{
			D3D12_GPU_VIRTUAL_ADDRESS cbAddress = materialCB->GetGPUVirtualAddress();

			// Offset to the ith object constant buffer in the buffer.
			cbAddress += i * materialCBByteSize;

			// Offset to the object cbv in the descriptor heap.
			int heapIndex = m_iMaterialCbvOffset + frameIndex * materialCount + i;
			auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_CbvSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
			handle.Offset(heapIndex, g_CbvSrvUavDescriptorSize);

			D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
			cbvDesc.BufferLocation = cbAddress;
			cbvDesc.SizeInBytes = materialCBByteSize;	// 256바이트의 정수배로 정렬되어야한다.

			pDevice->CreateConstantBufferView(&cbvDesc, handle);
		}
	}


	UINT passCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(PassConstants));

	// Last three descriptors are the pass CBVs for each frame resource.
	for (int frameIndex = 0; frameIndex < NUM_FRAME_RESOURCE; ++frameIndex)
	{
		auto passCB = m_vFrameResources[frameIndex]->PassCB->Resource();
		D3D12_GPU_VIRTUAL_ADDRESS cbAddress = passCB->GetGPUVirtualAddress();

		// Offset to the pass cbv in the descriptor heap.
		int heapIndex = mPassCbvOffset + frameIndex;
		auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_CbvSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
		handle.Offset(heapIndex, g_CbvSrvUavDescriptorSize);

		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
		cbvDesc.BufferLocation = cbAddress;
		cbvDesc.SizeInBytes = passCBByteSize;

		pDevice->CreateConstantBufferView(&cbvDesc, handle);
	}
}

void CObject_TextureShader::Initialize(ID3D12Device * pDevice, ID3D12GraphicsCommandList * pd3dCommandList, const WCHAR * pszShaderFileName)
{
	CShader::Initialize_ShaderFileName(pszShaderFileName);

	CreatePipeLineParts(1);

	CreateDescriptorHeaps(pDevice);
	CreateShaderVariables(pDevice, pd3dCommandList);
	CreateFrameResources(pDevice);
	CreateConstantBufferViews(pDevice);
	CreateRootSignature(pDevice);
	CreatePSO(pDevice, 1, PSO_OBJECT);

	BuildObjects();
}

void CObject_TextureShader::UpdateShaderVariables(const CTimer & timer, CCamera * pCamera)
{
}

void CObject_TextureShader::ReleaseShaderVariables()
{
}

void CObject_TextureShader::UpdateMainPassCB(CCamera * pCamera)
{
}

void CObject_TextureShader::UpdateObjectCBs(const CTimer & timer)
{
}

void CObject_TextureShader::BuildObjects(void)
{
}

void CObject_TextureShader::OnPrepareRender(ID3D12GraphicsCommandList * pd3dCommandList)
{
	ID3D12DescriptorHeap* descriptorHeaps[] = { m_CbvSrvDescriptorHeap.Get() };
	pd3dCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps); //pd3dCommandList->SetDescriptorHeaps(1, m_CbvHeap.GetAddressOf());

	//pd3dCommandList->SetGraphicsRootSignature(m_RootSignature->Get());
	if (m_RootSignature[PSO_OBJECT])		// 루트서명을 Set하는 순간 모든 바인딩이 사라진다. 여기서부터 새 루트서명이 기대하는 자원을 묶기 시작한다.
		pd3dCommandList->SetGraphicsRootSignature(m_RootSignature[PSO_OBJECT].Get());

	if (m_pPSOs[PSO_OBJECT])
		pd3dCommandList->SetPipelineState(m_pPSOs[PSO_OBJECT].Get());

	int passCbvIndex = mPassCbvOffset + m_CurrFrameResourceIndex;
	auto passCbvHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(m_CbvSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	passCbvHandle.Offset(passCbvIndex, g_CbvSrvUavDescriptorSize);
	pd3dCommandList->SetGraphicsRootDescriptorTable(0, passCbvHandle);		// 서술자 테이블을 파이프라인에 묶는다. // Pass RootDescriptor는 0번
}

void CObject_TextureShader::Render(ID3D12GraphicsCommandList * pd3dCommandList, CCamera * pCamera, UINT64 nFenceValue)
{
	OnPrepareRender(pd3dCommandList);
	for (UINT index = 0; index < m_vpObjects.size(); ++index)
	{
		CGameObject* pObject = *(m_vpObjects[index]);

		if (pObject == nullptr)
			continue;

		UINT cbvIndex = m_CurrFrameResourceIndex * (UINT)m_vpObjects.size() + index;
		auto cbvHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(m_CbvSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
		cbvHandle.Offset(cbvIndex, g_CbvSrvUavDescriptorSize);

		pd3dCommandList->SetGraphicsRootDescriptorTable(1, cbvHandle);// Object RootDescriptor는 1번

		pObject->Render(pd3dCommandList);
	}

	m_CurrFrameResource->Fence = nFenceValue;
}

array<const CD3DX12_STATIC_SAMPLER_DESC, ARRAY_SIZE> CObject_TextureShader::GetStaticSamplers()
{
	// Applications usually only need a handful of samplers.  So just define them all up front
	// and keep them available as part of the root signature.  

	const CD3DX12_STATIC_SAMPLER_DESC pointWrap(
		0, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC pointClamp(
		1, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC linearWrap(
		2, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC linearClamp(
		3, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicWrap(
		4, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressW
		0.0f,                             // mipLODBias
		8);                               // maxAnisotropy

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicClamp(
		5, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressW
		0.0f,                              // mipLODBias
		8);                                // maxAnisotropy

	return {
		pointWrap, pointClamp,
		linearWrap, linearClamp,
		anisotropicWrap, anisotropicClamp };
}
