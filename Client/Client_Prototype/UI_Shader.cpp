#include "UI_Shader.h"

#include "UIObject.h"

CUI_Shader::CUI_Shader()
{
}


CUI_Shader::~CUI_Shader()
{
}

int CUI_Shader::Update(const CTimer & timer, ID3D12Fence * pFence, CCamera * pCamera)
{
	CShader::Update(timer, pFence, pCamera);
	UpdateShaderVariables(timer, pCamera);

	return 0;
}

void CUI_Shader::Render(ID3D12GraphicsCommandList * pd3dCommandList, CCamera * pCamera, UINT64 nFenceValue)
{
	OnPrepareRender(pd3dCommandList);
	UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));
	UINT matCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(MaterialConstants));
	auto objectCB = m_CurrFrameResource->ObjectCB->Resource();
	auto matCB = m_CurrFrameResource->MaterialCB->Resource();
	for (UINT index = 0; index < m_vpObjects.size(); ++index)
	{
		CModelObject* pObject = (CModelObject*)*(m_vpObjects[index]);

		if (pObject == nullptr)
			continue;

		CMaterial* pMaterial = pObject->GetMaterial();
		if (pMaterial == nullptr)
			continue;

		GPUSRVHANDLEMAP gpuHandle = m_mapTextureGPUSrvHandle.find(pMaterial->Name);
		if (gpuHandle == m_mapTextureGPUSrvHandle.end())
			continue;

		D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = objectCB->GetGPUVirtualAddress() + index * objCBByteSize;
		D3D12_GPU_VIRTUAL_ADDRESS matCBAddress = matCB->GetGPUVirtualAddress() + index * matCBByteSize;

		pd3dCommandList->SetGraphicsRootConstantBufferView(1, objCBAddress);
		pd3dCommandList->SetGraphicsRootConstantBufferView(2, matCBAddress);
		pd3dCommandList->SetGraphicsRootDescriptorTable(3, gpuHandle->second);

		pObject->Render(pd3dCommandList);
	}

	m_CurrFrameResource->Fence = nFenceValue;
}

void CUI_Shader::Initialize(ID3D12Device * pDevice, ID3D12GraphicsCommandList * pd3dCommandList, const WCHAR * pszShaderFileName, vector<CGameObject*>& vObjects)
{
	CShader::Initialize_ShaderFileName(pszShaderFileName);

	CreatePipeLineParts(1);
	CreateRootSignature(pDevice);
	BuildObjects(vObjects, pDevice, pd3dCommandList);
	CreateDescriptorHeaps(pDevice);
	CreateShaderVariables(pDevice, pd3dCommandList);
	CreateFrameResources(pDevice);
	CreateConstantBufferViews(pDevice);

	CreatePSO(pDevice, 1, PSO_OBJECT);
}

void CUI_Shader::LoadTextures(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
}

void CUI_Shader::UpdateMaterialCB(void)
{
}

void CUI_Shader::UpdateUICB(void)
{

}

D3D12_INPUT_LAYOUT_DESC CUI_Shader::CreateInputLayout()
{
	UINT nInputElementDescs = 7;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	//정점 정보를 위한 입력 원소이다. 현재 UI Data의 구조는 차례대로 pos, uv

	pd3dInputElementDescs[0] = { "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	0,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[2] = { "TANGENT",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[3] = { "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0,	36, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[4] = { "BORNINDEX",	0, DXGI_FORMAT_R32G32B32A32_UINT,	0,	44, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[5] = { "WEIGHT",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	60, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[6] = { "TEXINDEX",	0, DXGI_FORMAT_R32_UINT,			0,	72, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	/*pd3dInputElementDescs[0] = { "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	0,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "TEXCOORD",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };*/

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;
	return(d3dInputLayoutDesc);
}

D3D12_RASTERIZER_DESC CUI_Shader::CreateRasterizerState()
{
	D3D12_RASTERIZER_DESC d3dRasterizerDesc;
	::ZeroMemory(&d3dRasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));

	d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	//d3dRasterizerDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;

	//d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	//d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_FRONT;

	d3dRasterizerDesc.FrontCounterClockwise = FALSE;
	d3dRasterizerDesc.DepthBias = 0;
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias = 0.0f;

	d3dRasterizerDesc.DepthClipEnable = TRUE;
	//d3dRasterizerDesc.DepthClipEnable = FALSE;

	d3dRasterizerDesc.MultisampleEnable = FALSE;
	d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
	d3dRasterizerDesc.ForcedSampleCount = 0;
	d3dRasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
	return(d3dRasterizerDesc);
}

D3D12_BLEND_DESC CUI_Shader::CreateBlendState()
{
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));

	d3dBlendDesc.AlphaToCoverageEnable = false;
	d3dBlendDesc.IndependentBlendEnable = false;
	d3dBlendDesc.RenderTarget[0].BlendEnable = true;
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = false;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return d3dBlendDesc;
}

D3D12_DEPTH_STENCIL_DESC CUI_Shader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC desc;
	::ZeroMemory(&desc, sizeof(D3D12_DEPTH_STENCIL_DESC));

	desc.DepthEnable = false;
	desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	desc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	desc.StencilEnable = false;
	desc.StencilReadMask = 0x00;
	desc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	desc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	desc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	desc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_LESS;
	desc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	desc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	desc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	desc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_LESS;

	return desc;
}

void CUI_Shader::CreatePSO(ID3D12Device * pd3dDevice, UINT nRenderTargets, int index)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	::ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	ID3DBlob *pd3dVertexShaderBlob = NULL, *pd3dPixelShaderBlob = NULL;
	//
	// PSO for opaque objects.
	//
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	psoDesc.InputLayout = CreateInputLayout();
	psoDesc.pRootSignature = m_RootSignature[index].Get();

	psoDesc.VS = CreateVertexShader(&pd3dVertexShaderBlob, "VS");
	psoDesc.PS = CreatePixelShader(&pd3dPixelShaderBlob, "PS");

	psoDesc.RasterizerState = CreateRasterizerState();
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CreateDepthStencilState();
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	for (UINT i = 0; i < nRenderTargets; i++)
		psoDesc.RTVFormats[i] = m_Format;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	ThrowIfFailed(pd3dDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(m_pPSOs[index].GetAddressOf())));
	if (pd3dVertexShaderBlob) pd3dVertexShaderBlob->Release();
	if (pd3dPixelShaderBlob) pd3dPixelShaderBlob->Release();
}

void CUI_Shader::CreateFrameResources(ID3D12Device * pd3dDevice)
{
	for (int i = 0; i < NUM_FRAME_RESOURCE; ++i)
	{
		m_vFrameResources.push_back(std::make_unique<FrameResource>(pd3dDevice,
			1, (UINT)m_vpObjects.size()));
	}
}

void CUI_Shader::CreateDescriptorHeaps(ID3D12Device * pd3dDevice)
{
	UINT nNumDescriptor = 0;
	for (UINT i = 0; i < m_vpObjects.size(); ++i)
	{
		if (*m_vpObjects[i] == nullptr)
			continue;

		if ((*m_vpObjects[i])->GetMaterial() != nullptr)
		{
			nNumDescriptor++;
		}
	}
	//
	// Create the SRV heap.
	//
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.NumDescriptors = nNumDescriptor;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	ThrowIfFailed(pd3dDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_CbvSrvDescriptorHeap)));
	//
	// Fill out the heap with actual descriptors.
	//
	CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(m_CbvSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
	CD3DX12_GPU_DESCRIPTOR_HANDLE hGPUDescriptor(m_CbvSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	for (UINT i = 0; i < m_vpObjects.size(); ++i)
	{
		CModelObject* pObject = *m_vpObjects[i];
		if (pObject == nullptr)
			continue;
		CMaterial* pMaterial = pObject->GetMaterial();
		if (pMaterial == nullptr)
			continue;
		CPUSRVHANDLEMAP iter = m_mapTextureCPUSrvHandle.find(pMaterial->Name);
		if (iter != m_mapTextureCPUSrvHandle.end())
			continue;

		ID3D12Resource* pResource = pMaterial->m_pTexture->m_pd3dTextures.Get();
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = pObject->GetMaterial()->m_pTexture->m_srvDesc;

		pd3dDevice->CreateShaderResourceView(pResource, &srvDesc, hDescriptor);

		m_mapTextureCPUSrvHandle.insert(make_pair(pMaterial->Name, hDescriptor));
		m_mapTextureGPUSrvHandle.insert(make_pair(pMaterial->Name, hGPUDescriptor));

		hDescriptor.Offset(g_CbvSrvUavDescriptorSize);
		hGPUDescriptor.Offset(g_CbvSrvUavDescriptorSize);
	}
}

void CUI_Shader::CreateRootSignature(ID3D12Device * pd3dDevice)
{
	UINT numSlot = 4;

	CD3DX12_DESCRIPTOR_RANGE texTable;
	texTable.Init(
		D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
		1,  // number of descriptors
		0); // register t0

	// Root parameter can be a table, root descriptor or root constants.
	CD3DX12_ROOT_PARAMETER slotRootParameter[4];

	// Perfomance TIP: Order from most frequent to least frequent.

	slotRootParameter[0].InitAsConstantBufferView(0); // register b0 : pass
	slotRootParameter[1].InitAsConstantBufferView(1); // register b1 : objectcb
	slotRootParameter[2].InitAsConstantBufferView(2); // register b2 : material
	slotRootParameter[3].InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_PIXEL);

	// A root signature is an array of root parameters.

	auto staticSamplers = GetStaticSamplers();

	// A root signature is an array of root parameters.

	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(numSlot, slotRootParameter,
		(UINT)staticSamplers.size(), staticSamplers.data(),
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

void CUI_Shader::CreateShader(ID3D12Device * pd3dDevice, ID3D12RootSignature * pd3dGraphicsRootSignature)
{
}

void CUI_Shader::CreateShaderVariables(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
}

void CUI_Shader::CreateConstantBufferViews(ID3D12Device * pDevice)
{
}

void CUI_Shader::UpdateShaderVariables(const CTimer & timer, CCamera * pCamera)
{
	UpdateMainPassCB(pCamera);
	auto currObjectCB = m_CurrFrameResource->ObjectCB.get();
	auto currMaterialCB = m_CurrFrameResource->MaterialCB.get();

	for (UINT i = 0; i < m_vpObjects.size(); ++i)
	{
		CModelObject* pObject = *m_vpObjects[i];
		if (pObject == nullptr)
			continue;

		if (pObject->GetFramesDirty() > 0)
		{
			currObjectCB->CopyData(i, pObject->GetObjectConstants());

			CMaterial* material = pObject->GetMaterial();
			if (material == nullptr)
				continue;

			XMMATRIX matTransform = XMLoadFloat4x4(&material->MatTransform);

			MaterialConstants matConstants = material->getMaterialConst();
			currMaterialCB->CopyData(i, matConstants);

			pObject->DecreaseFramesDirty();
		}
	}
}

void CUI_Shader::ReleaseShaderVariables()
{
}

void CUI_Shader::UpdateMainPassCB(CCamera * pCamera)
{
	XMMATRIX view = XMLoadFloat4x4(&pCamera->GetViewMatrix());
	XMMATRIX proj = XMLoadFloat4x4(&pCamera->GetProjectionMatrix());

	XMMATRIX viewProj = XMMatrixMultiply(view, proj);
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);
	XMMATRIX invProj = XMMatrixInverse(&XMMatrixDeterminant(proj), proj);
	XMMATRIX invViewProj = XMMatrixInverse(&XMMatrixDeterminant(viewProj), viewProj);

	XMStoreFloat4x4(&mMainPassCB.View, XMMatrixTranspose(view));
	XMStoreFloat4x4(&mMainPassCB.InvView, XMMatrixTranspose(invView));
	XMStoreFloat4x4(&mMainPassCB.Proj, XMMatrixTranspose(proj));
	XMStoreFloat4x4(&mMainPassCB.InvProj, XMMatrixTranspose(invProj));
	XMStoreFloat4x4(&mMainPassCB.ViewProj, XMMatrixTranspose(viewProj));
	XMStoreFloat4x4(&mMainPassCB.InvViewProj, XMMatrixTranspose(invViewProj));
	mMainPassCB.EyePosW = pCamera->GetPosition();
	mMainPassCB.RenderTargetSize = XMFLOAT2((float)gClientWidth, (float)gClientHeight);
	mMainPassCB.InvRenderTargetSize = XMFLOAT2(1.0f / gClientWidth, 1.0f / gClientHeight);
	mMainPassCB.NearZ = 1.0f;
	mMainPassCB.FarZ = 1000.0f;
	//mMainPassCB.TotalTime = timer.TotalTime();
	//mMainPassCB.DeltaTime = timer.DeltaTime();
	mMainPassCB.AmbientLight = { 0.80f, 0.80f, 0.80f, 1.0f };
	mMainPassCB.Lights[0].Direction = { 0.57735f, -0.57735f, 0.57735f };
	mMainPassCB.Lights[0].Strength = { 0.8f, 0.8f, 0.8f };
	mMainPassCB.Lights[1].Direction = { -0.57735f, -0.57735f, 0.57735f };
	mMainPassCB.Lights[1].Strength = { 0.4f, 0.4f, 0.4f };
	mMainPassCB.Lights[2].Direction = { 0.0f, -0.707f, -0.707f };
	mMainPassCB.Lights[2].Strength = { 0.2f, 0.2f, 0.2f };

	auto currPassCB = m_CurrFrameResource->PassCB.get();
	currPassCB->CopyData(0, mMainPassCB);
}

void CUI_Shader::UpdateObjectCBs()
{
	auto currObjectCB = m_CurrFrameResource->ObjectCB.get();
	for (UINT index = 0; index < m_vpObjects.size(); ++index)
	{
		CGameObject* pObject = *(m_vpObjects[index]);

		if (pObject == nullptr)
			continue;
		// ObjectCB를 갱신할 필요가 생겼다면 갱신한다.
		if (pObject->GetFramesDirty() > 0)
		{
			currObjectCB->CopyData(index, pObject->GetObjectConstants());
			pObject->DecreaseFramesDirty();
		}
	}
}

void CUI_Shader::BuildObjects(vector<CGameObject*>& vObjects, ID3D12Device * pDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
	CUI_Timer* pUIObject = nullptr;
	CTransform* pTransform = nullptr;

	pUIObject = new CUI_Timer;
	pUIObject->Initialize(L"Texture_HPedge", pDevice, pd3dCommandList);
	vObjects.push_back(pUIObject);
	Push_Object(pUIObject);

}

void CUI_Shader::OnPrepareRender(ID3D12GraphicsCommandList * pd3dCommandList)
{
	ID3D12DescriptorHeap* descriptorHeaps[] = { m_CbvSrvDescriptorHeap.Get() };
	pd3dCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps); //pd3dCommandList->SetDescriptorHeaps(1, m_CbvHeap.GetAddressOf());

	//pd3dCommandList->SetGraphicsRootSignature(m_RootSignature->Get());
	if (m_RootSignature[PSO_OBJECT])		// 루트서명을 Set하는 순간 모든 바인딩이 사라진다. 여기서부터 새 루트서명이 기대하는 자원을 묶기 시작한다.
		pd3dCommandList->SetGraphicsRootSignature(m_RootSignature[PSO_OBJECT].Get());

	if (m_pPSOs[PSO_OBJECT])
		pd3dCommandList->SetPipelineState(m_pPSOs[PSO_OBJECT].Get());

	auto passCB = m_CurrFrameResource->PassCB->Resource();
	pd3dCommandList->SetGraphicsRootConstantBufferView(0, passCB->GetGPUVirtualAddress());
}

array<const CD3DX12_STATIC_SAMPLER_DESC, 6> CUI_Shader::GetStaticSamplers()
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