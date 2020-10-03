#include "UI_Shader.h"

#include "UIObject.h"
#include "UI_HPBar.h"
#include "UI_HPBackground.h"
#include "UI_ReadyStart.h"
#include "UI_Timer.h"
#include "UI_WinLose.h"
#include "UI_TimerFont.h"

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
	

	for (int pso = 0; pso < PSO_END; pso++)
	{
		pd3dCommandList->SetPipelineState(m_pPSOs[pso].Get());

		auto passCB = m_CurrFrameResource->PassCB->Resource();
		pd3dCommandList->SetGraphicsRootConstantBufferView(0, passCB->GetGPUVirtualAddress());

		UINT UICBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(UI_Constants));
		UINT matCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(MaterialConstants));
		auto UICB = m_CurrFrameResource->UICB->Resource();
		auto matCB = m_CurrFrameResource->MaterialCB->Resource();

		for (UINT index = 0; index < m_vObjectVector[pso].size(); index++)
		{
			CUI_Object* pObject = (CUI_Object*)*(m_vObjectVector[pso][index]);

			if (pObject == nullptr)
				continue;

			CMaterial* pMaterial = pObject->GetMaterial();
			if (pMaterial == nullptr)
				continue;

			GPUSRVHANDLEMAP gpuHandle = m_mapTextureGPUSrvHandle.find(pMaterial->Name);
			if (gpuHandle == m_mapTextureGPUSrvHandle.end())
				continue;

		

			D3D12_GPU_VIRTUAL_ADDRESS UICBAddress = UICB->GetGPUVirtualAddress() + (pso * m_vObjectVector[pso].size() + index)  * UICBByteSize;

			pd3dCommandList->SetGraphicsRootConstantBufferView(1, UICBAddress);
			pd3dCommandList->SetGraphicsRootDescriptorTable(2, gpuHandle->second);

			pObject->Render(pd3dCommandList);
		}
	}
	
	m_CurrFrameResource->Fence = nFenceValue;
}

void CUI_Shader::Initialize(ID3D12Device * pDevice, ID3D12GraphicsCommandList * pd3dCommandList, const WCHAR * pszShaderFileName, vector<CGameObject*>& vObjects)
{
	CShader::Initialize_ShaderFileName(pszShaderFileName);

	m_nObjects = MAX_OBJECT * PSO_END;
	
	for (int i = 0; i < PSO_END; ++i)
	{
		m_vObjectVector[i] = vector<CGameObject**>(m_nObjects / PSO_END);
		for (auto& ppObject : m_vObjectVector[i])
		{
			ppObject = new CGameObject*;
			*ppObject = nullptr;
		}
	}

	CreatePipeLineParts(PSO_END);
	CreateRootSignature(pDevice);
	BuildObjects(vObjects, pDevice, pd3dCommandList);
	CreateDescriptorHeaps(pDevice);
	CreateShaderVariables(pDevice, pd3dCommandList);
	CreateFrameResources(pDevice);
	CreateConstantBufferViews(pDevice);

	for(int i = 0; i < PSO_END; i++)
		CreatePSO(pDevice, 1, i);
}

bool CUI_Shader::Push_Object(CGameObject * pObject, UINT PSOindex)
{
	for (UINT i = 0; i < m_vObjectVector[PSOindex].size(); ++i)
	{
		CGameObject** m_value_pObject = m_vObjectVector[PSOindex][i];
		if (*m_value_pObject == nullptr)
		{
			*m_value_pObject = pObject;
			return true;
		}
	}
	return false;
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
	UINT nInputElementDescs = 0;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	//정점 정보를 위한 입력 원소이다. UI Vertex의 정보를 비워서 넘긴다.  셰이더에서 다 처리할예정

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

	d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//d3dRasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
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

	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	psoDesc.InputLayout = CreateInputLayout();
	psoDesc.pRootSignature = m_RootSignature[0].Get();


	switch (index)
	{
	case PSO_Edge:
		psoDesc.VS = CreateVertexShader(&pd3dVertexShaderBlob, "VS");
		psoDesc.PS = CreatePixelShader(&pd3dPixelShaderBlob, "PS");
	
		break;
	case PSO_HPBar:
		psoDesc.VS = CreateVertexShader(&pd3dVertexShaderBlob, "VS_HPBar");
		psoDesc.PS = CreatePixelShader(&pd3dPixelShaderBlob, "PS");
		break;
	case PSO_Timer:
		psoDesc.VS = CreateVertexShader(&pd3dVertexShaderBlob, "VS_Timer");
		psoDesc.PS = CreatePixelShader(&pd3dPixelShaderBlob, "PS");
		break;
	default:
		psoDesc.VS = CreateVertexShader(&pd3dVertexShaderBlob, "VS");
		psoDesc.PS = CreatePixelShader(&pd3dPixelShaderBlob, "PS");
	}
	
	psoDesc.RasterizerState = CreateRasterizerState();
	psoDesc.BlendState = CreateBlendState();
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
			1, m_nObjects));
	}
}

void CUI_Shader::CreateDescriptorHeaps(ID3D12Device * pd3dDevice)
{
	UINT nNumDescriptor = m_nObjects;

	/*for (UINT PSOindex = 0; PSOindex < PSO_END; ++PSOindex)
	{
		for (UINT i = 0; i < m_vObjectVector[PSOindex].size(); ++i)
		{
			CUI_Object* pObject = dynamic_cast<CUI_Object*>(*m_vObjectVector[PSOindex][i]);
			if (pObject == nullptr)
				continue;

			if (pObject->GetMaterial() != nullptr)
			{
				nNumDescriptor++;
			}
		}
	}*/
	
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
	for (UINT PSOindex = 0; PSOindex < PSO_END; ++PSOindex)
	{
		for (UINT i = 0; i < m_vObjectVector[PSOindex].size(); ++i)
		{
			CUI_Object* pObject = dynamic_cast<CUI_Object*>(*m_vObjectVector[PSOindex][i]);


			if (pObject == nullptr)
				continue;
			vector<CMaterial*>* vMaterial = pObject->GetAllMaterial();
			if (vMaterial->empty())
				continue;

			for (int i = 0; i < vMaterial->size(); ++i)
			{
				CMaterial* pMaterial = (*vMaterial)[i];
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
	}
	
}

void CUI_Shader::CreateRootSignature(ID3D12Device * pd3dDevice)
{
	UINT numSlot = 3;

	CD3DX12_DESCRIPTOR_RANGE texTable;
	texTable.Init(
		D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
		1,  // number of descriptors
		0); // register t0

	// Root parameter can be a table, root descriptor or root constants.
	CD3DX12_ROOT_PARAMETER slotRootParameter[3];

	// Perfomance TIP: Order from most frequent to least frequent.

	slotRootParameter[0].InitAsConstantBufferView(0); // register b0 : pass
	slotRootParameter[1].InitAsConstantBufferView(1); // register b1 : UIcb
	slotRootParameter[2].InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_PIXEL);

	// A root signature is an array of root parameters.

	auto staticSamplers = GetStaticSamplers();

	D3D12_STATIC_SAMPLER_DESC d3dSamplerDesc;
	::ZeroMemory(&d3dSamplerDesc, sizeof(D3D12_STATIC_SAMPLER_DESC));
	d3dSamplerDesc = CD3DX12_STATIC_SAMPLER_DESC(
		0,
		D3D12_FILTER_MIN_MAG_MIP_LINEAR,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		0.0f,
		1,
		D3D12_COMPARISON_FUNC_ALWAYS,
		D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE,
		0.0f,
		D3D12_FLOAT32_MAX,
		D3D12_SHADER_VISIBILITY_PIXEL
	);

	// A root signature is an array of root parameters.

	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(numSlot, slotRootParameter,
		(UINT)staticSamplers.size(), staticSamplers.data(),
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	rootSigDesc.NumStaticSamplers = 1;
	rootSigDesc.pStaticSamplers = &d3dSamplerDesc;

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
		IID_PPV_ARGS(m_RootSignature[0].GetAddressOf())));
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

	auto currUICB = m_CurrFrameResource->UICB.get();
	for (UINT PSOindex = 0; PSOindex < PSO_END; ++PSOindex)
	{
		for (UINT i = 0; i < m_vObjectVector[PSOindex].size(); ++i)
		{
			CUI_Object* pObject = dynamic_cast<CUI_Object*>(*m_vObjectVector[PSOindex][i]);
			if (pObject == nullptr)
				continue;

			if (pObject->GetFramesDirty() > 0)
			{
				currUICB->CopyData(PSOindex * m_vObjectVector[PSOindex].size() + i, pObject->Get_UIConstants());		// updateUICB
				
				pObject->DecreaseFramesDirty();
			}
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
	CUI_Object* pUIObject = nullptr;

	// Flag Timer
	pUIObject = new CUI_Timer;
	pUIObject->Initialize();
	pUIObject->SetPos(XMFLOAT2(FRAME_BUFFER_WIDTH * 0.3f, FRAME_BUFFER_HEIGHT * 0.9f));
	vObjects.push_back(pUIObject);
	Push_Object(pUIObject, PSO_Timer);

	// Limit Timer
	pUIObject = new CUI_Timer;
	pUIObject->Initialize();
	pUIObject->SetPos(XMFLOAT2(FRAME_BUFFER_WIDTH * 0.8f, FRAME_BUFFER_HEIGHT * 0.9f));
	vObjects.push_back(pUIObject);
	Push_Object(pUIObject, PSO_Timer);

	pUIObject = new CUI_ReadyStart;
	pUIObject->Initialize();
	pUIObject->SetInvisible(true);
	vObjects.push_back(pUIObject);
	Push_Object(pUIObject, PSO_ETC);

	pUIObject = new CUI_WinLose;
	pUIObject->Initialize();
	pUIObject->SetInvisible(true);
	vObjects.push_back(pUIObject);
	Push_Object(pUIObject, PSO_ETC);

	int index = 0;
	for (int width = 0; width < 4; ++width)
	{
		for (int height = 0; height < 2; ++height)
		{
			// HP Edge
			pUIObject = new CUI_HPBackground;
			pUIObject->Initialize();
			pUIObject->SetPos(XMFLOAT2(175 + 300 * width , 150 + height * -100));
			pUIObject->SetMaterialIndex(index);
			vObjects.push_back(pUIObject);
			Push_Object(pUIObject, PSO_Edge);

			// HP Bar
			pUIObject = new CUI_HPBar;
			((CUI_HPBar*)pUIObject)->Initialize();
			pUIObject->SetPos(XMFLOAT2(175 + 300 * width, 150 + height * -100));
			pUIObject->SetTarget(m_pObject_Manager->Get_Object(CObject_Manager::LAYER_PLAYER, index++));
			vObjects.push_back(pUIObject);
			Push_Object(pUIObject, PSO_HPBar);
		}
	}

	pUIObject = new CUI_TimerFont;
	pUIObject->Initialize();
	pUIObject->SetMaterialIndex(0);
	pUIObject->SetPos(XMFLOAT2(FRAME_BUFFER_WIDTH * 0.1f, FRAME_BUFFER_HEIGHT * 0.9f));
	vObjects.push_back(pUIObject);
	Push_Object(pUIObject, PSO_Edge);

	pUIObject = new CUI_TimerFont;
	pUIObject->Initialize();
	pUIObject->SetMaterialIndex(1);
	pUIObject->SetPos(XMFLOAT2(FRAME_BUFFER_WIDTH * 0.6f, FRAME_BUFFER_HEIGHT * 0.9f));
	vObjects.push_back(pUIObject);
	Push_Object(pUIObject, PSO_Edge);

}

void CUI_Shader::OnPrepareRender(ID3D12GraphicsCommandList * pd3dCommandList)
{

	if (m_RootSignature[0])		// 루트서명을 Set하는 순간 모든 바인딩이 사라진다. 여기서부터 새 루트서명이 기대하는 자원을 묶기 시작한다.
		pd3dCommandList->SetGraphicsRootSignature(m_RootSignature[0].Get());

	ID3D12DescriptorHeap* descriptorHeaps[] = { m_CbvSrvDescriptorHeap.Get() };
	pd3dCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps); //pd3dCommandList->SetDescriptorHeaps(1, m_CbvHeap.GetAddressOf());

	
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