#include "Shader.h"
#include "Timer.h"
#include "Camera.h"

CShader::CShader()
{
}


CShader::~CShader()
{
	if (m_ppd3dPipelineStates)
	{
		for (int i = 0; i < m_nPipelineStates; i++)
			if (m_ppd3dPipelineStates[i])
				m_ppd3dPipelineStates[i]->Release();
		delete[] m_ppd3dPipelineStates;
	}
}

void CShader::Update(const CTimer& timer, ID3D12Fence * pFence, ID3D12GraphicsCommandList * cmdList, CCamera* pCamera)
{
	m_CurrFrameResourceIndex = (m_CurrFrameResourceIndex + 1) % NUM_FRAME_RESOURCE;
	m_CurrFrameResource = m_vFrameresources[m_CurrFrameResourceIndex].get();

	//gpu�� ���� ������ �ڿ��� ����� �� ó���ߴ��� Ȯ��.
	// ���� �� ó�������ʾ����� gpu�� �� �潺�������� ��ɵ��� ó���Ҷ����� ��ٸ���.
	// �ᱹ ���� ��İ� �ٺ������� �ٸ��� ������ ��� ó���� �ڿ��� 3���� ����� �� GPU�۾��� �Ϸ�ɋ����� CPU�� �����ʰ� �ٸ� ��� ������ �Ѵٴ� ���������ִ�.
	if (m_CurrFrameResource->Fence != 0 && pFence->GetCompletedValue() < m_CurrFrameResource->Fence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
		ThrowIfFailed(pFence->SetEventOnCompletion(m_CurrFrameResource->Fence, eventHandle));
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}

	UpdateShaderVariables(timer, cmdList, pCamera);

	
}

bool CShader::Push_Object(CGameObject * pObject)
{
	for (UINT i = 0; i < m_vObjects.size(); ++i)
	{
		if (m_vObjects[i] == nullptr)
		{
			m_vObjects[i] = pObject;
			return true;
		}
	}
	return false;
}

D3D12_INPUT_LAYOUT_DESC CShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	//���� ������ ���� �Է� �����̴�.
	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;
	return(d3dInputLayoutDesc);
}

D3D12_RASTERIZER_DESC CShader::CreateRasterizerState()
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

D3D12_BLEND_DESC CShader::CreateBlendState()
{
	return CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	// �Ʒ��� Ŀ�����ڵ�
	D3D12_BLEND_DESC d3dBlendDesc;
	::ZeroMemory(&d3dBlendDesc, sizeof(D3D12_BLEND_DESC));
	d3dBlendDesc.AlphaToCoverageEnable = FALSE;			// FALSE
	d3dBlendDesc.IndependentBlendEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].BlendEnable = TRUE;	// FALSE
	d3dBlendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	d3dBlendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;	//D3D12_BLEND_ONE
	d3dBlendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;//D3D12_BLEND_ZERO
	d3dBlendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3dBlendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3dBlendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3dBlendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3dBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	return(d3dBlendDesc);
}

D3D12_DEPTH_STENCIL_DESC CShader::CreateDepthStencilState()
{
	return CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	// �Ʒ��� Ŀ�����ڵ�
	D3D12_DEPTH_STENCIL_DESC d3dDepthStencilDesc;
	::ZeroMemory(&d3dDepthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3dDepthStencilDesc.DepthEnable = TRUE;
	d3dDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3dDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3dDepthStencilDesc.StencilEnable = FALSE;
	d3dDepthStencilDesc.StencilReadMask = 0x00;
	d3dDepthStencilDesc.StencilWriteMask = 0x00;
	d3dDepthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3dDepthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3dDepthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	return(d3dDepthStencilDesc);
}

D3D12_SHADER_BYTECODE CShader::CreateVertexShader(ID3DBlob ** ppd3dShaderBlob)
{
	wchar_t filename[100] = L"Shaders\\color.hlsl";
	return(CShader::CompileShaderFromFile(filename, "VS", "vs_5_1", ppd3dShaderBlob));
	
}

D3D12_SHADER_BYTECODE CShader::CreatePixelShader(ID3DBlob ** ppd3dShaderBlob)
{
	wchar_t filename[100] = L"Shaders\\color.hlsl";
	return(CShader::CompileShaderFromFile(filename, "PS", "ps_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CShader::CompileShaderFromFile(WCHAR * pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderProfile, ID3DBlob ** ppd3dShaderBlob)
{
	UINT nCompileFlags = 0;
#if defined(_DEBUG)
	nCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	::D3DCompileFromFile(pszFileName, NULL, NULL, pszShaderName, pszShaderProfile, nCompileFlags, 0, ppd3dShaderBlob, NULL);
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = (*ppd3dShaderBlob)->GetBufferSize();
	d3dShaderByteCode.pShaderBytecode = (*ppd3dShaderBlob)->GetBufferPointer();
	return(d3dShaderByteCode);
}

void CShader::CreatePipeLineParts(void)
{
	m_nPSO = 1;
	if (m_nPSO > 0) {
		m_pPSOs = new ComPtr<ID3D12PipelineState>[m_nPSO];
		m_RootSignature = new ComPtr<ID3D12RootSignature>[m_nPSO];

	}
}

void CShader::CreatePSO(ID3D12Device * pd3dDevice, UINT nRenderTargets, int index)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	::ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	ID3DBlob *pd3dVertexShaderBlob = NULL, *pd3dPixelShaderBlob = NULL;
	psoDesc.InputLayout = CreateInputLayout();
	psoDesc.pRootSignature = m_RootSignature[index].Get();
	psoDesc.VS = CreateVertexShader(&pd3dVertexShaderBlob);
	psoDesc.PS = CreatePixelShader(&pd3dPixelShaderBlob);
	psoDesc.RasterizerState = CreateRasterizerState();
	psoDesc.BlendState = CreateBlendState();
	psoDesc.DepthStencilState = CreateDepthStencilState();
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = nRenderTargets;
	for (UINT i = 0; i < nRenderTargets; i++)
		psoDesc.RTVFormats[i] = m_Format;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	ThrowIfFailed(pd3dDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(m_pPSOs[index].GetAddressOf())));
	if (pd3dVertexShaderBlob) pd3dVertexShaderBlob->Release();
	if (pd3dPixelShaderBlob) pd3dPixelShaderBlob->Release();
	//D3D12_GRAPHICS_PIPELINE_STATE_DESC PsoDesc;

	////
	//// PSO for opaque objects.
	////
	//ZeroMemory(&PsoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	//PsoDesc.InputLayout = CreateInputLayout();
	//PsoDesc.pRootSignature = m_RootSignature->Get();

	//ID3DBlob *pd3dVertexShaderBlob = NULL, *pd3dPixelShaderBlob = NULL;
	//PsoDesc.VS = CreateVertexShader(&pd3dVertexShaderBlob);
	//PsoDesc.PS = CreatePixelShader(&pd3dPixelShaderBlob);

	//PsoDesc.RasterizerState = CreateRasterizerState(); //CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	//PsoDesc.BlendState = CreateBlendState();// CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	//PsoDesc.DepthStencilState = CreateDepthStencilState();//CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	//PsoDesc.SampleMask = UINT_MAX;
	//PsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//PsoDesc.NumRenderTargets = m_nNumRenderTarget;
	//for(UINT i = 0; i < m_nNumRenderTarget; ++i)
	//	PsoDesc.RTVFormats[i] = m_Format;
	//PsoDesc.SampleDesc.Count = 1;
	///*PsoDesc.SampleDesc.Count = m_b4xMsaaState ? 4 : 1;
	//PsoDesc.SampleDesc.Quality = m_b4xMsaaState ? (m_n4xMsaaQuality - 1) : 0;*/

	//PsoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;//DXGI_FORMAT_D24_UNORM_S8_UINT

	//ThrowIfFailed(pd3dDevice->CreateGraphicsPipelineState(&PsoDesc, IID_PPV_ARGS(&mPSOs["opaque"])));

	//if (pd3dVertexShaderBlob) pd3dVertexShaderBlob->Release();
	//if (pd3dPixelShaderBlob) pd3dPixelShaderBlob->Release();
}

void CShader::CreateFrameResources(ID3D12Device * pd3dDevice)
{
	for (int i = 0; i < NUM_FRAME_RESOURCE; ++i)
	{
		m_vFrameresources.push_back(std::make_unique<FrameResource>(pd3dDevice,
			1, (UINT)m_vObjects.size()));
	}
}

void CShader::CreateDescriptorHeaps(ID3D12Device* pd3dDevice)
{
	UINT objCount = (UINT)m_vObjects.size();

	// Need a CBV descriptor for each object for each frame resource,
	// +1 for the perPass CBV for each frame resource.
	UINT numDescriptors = (objCount + 1) * NUM_FRAME_RESOURCE;

	// Save an offset to the start of the pass CBVs.  These are the last 3 descriptors.
	// ������ 3���� ��ũ����(������)�� �н��� CBV�� ���Ѱ��̴�.
	mPassCbvOffset = objCount * NUM_FRAME_RESOURCE;

	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
	cbvHeapDesc.NumDescriptors = numDescriptors;
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.NodeMask = 0;
	ThrowIfFailed(pd3dDevice->CreateDescriptorHeap(&cbvHeapDesc,						
		IID_PPV_ARGS(&m_CbvHeap)));


}

void CShader::CreateRootSignature(ID3D12Device * pd3dDevice)
{
	CD3DX12_DESCRIPTOR_RANGE cbvTable[2];
	cbvTable[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);		// for PassCB
	cbvTable[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);		// for ObjectCB

	// Root parameter can be a table, root descriptor or root constants.
	CD3DX12_ROOT_PARAMETER slotRootParameter[2];

	// Create root CBVs.
	slotRootParameter[0].InitAsDescriptorTable(1, &cbvTable[0]);
	slotRootParameter[1].InitAsDescriptorTable(1, &cbvTable[1]);

	// A root signature is an array of root parameters.
	// numparameters / rootParameter / numStaticSampler / pStaticSamplers / Flag
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(2, slotRootParameter, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

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

void CShader::CreateShader(ID3D12Device * pd3dDevice, ID3D12RootSignature * pd3dGraphicsRootSignature)
{
	ID3DBlob *pd3dVertexShaderBlob = NULL, *pd3dPixelShaderBlob = NULL;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3dPipelineStateDesc;
	::ZeroMemory(&d3dPipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	d3dPipelineStateDesc.pRootSignature = pd3dGraphicsRootSignature;
	d3dPipelineStateDesc.VS = CreateVertexShader(&pd3dVertexShaderBlob);
	d3dPipelineStateDesc.PS = CreatePixelShader(&pd3dPixelShaderBlob);
	d3dPipelineStateDesc.RasterizerState = CreateRasterizerState();
	d3dPipelineStateDesc.BlendState = CreateBlendState();
	d3dPipelineStateDesc.DepthStencilState = CreateDepthStencilState();
	d3dPipelineStateDesc.InputLayout = CreateInputLayout();
	d3dPipelineStateDesc.SampleMask = UINT_MAX;
	d3dPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	d3dPipelineStateDesc.NumRenderTargets = 1;
	d3dPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dPipelineStateDesc.SampleDesc.Count = 1;
	d3dPipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	pd3dDevice->CreateGraphicsPipelineState(&d3dPipelineStateDesc,
		__uuidof(ID3D12PipelineState), (void **)&m_ppd3dPipelineStates[0]);
	if (pd3dVertexShaderBlob) pd3dVertexShaderBlob->Release();
	if (pd3dPixelShaderBlob) pd3dPixelShaderBlob->Release();
	if (d3dPipelineStateDesc.InputLayout.pInputElementDescs) delete[]
		d3dPipelineStateDesc.InputLayout.pInputElementDescs;
}

void CShader::CreateShaderVariables(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{

}

void CShader::Initialize(ID3D12Device * pDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
	
	CreatePipeLineParts();

	m_nObjects = MAX_OBJECT;
	m_vObjects = vector<CGameObject*>(m_nObjects);

	CreateDescriptorHeaps(pDevice);		//CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, m_nObjects, 1);
	CreateShaderVariables(pDevice, pd3dCommandList);
	CreateFrameResources(pDevice); //CreateConstantBufferViews(pd3dDevice, pd3dCommandList, m_nObjects, m_ObjectCB->Resource(), D3DUtil::CalcConstantBufferByteSize(sizeof(CB_GAMEOBJECT_INFO)));	

	CreateRootSignature(pDevice);
	CreatePSO(pDevice, 1, PSO_OBJECT);

	BuildObjects();
}

void CShader::UpdateShaderVariables(const CTimer& timer, ID3D12GraphicsCommandList * pd3dCommandList, CCamera* pCamera)
{
	UpdateObjectCBs(timer);
	UpdateMainPassCB(timer, pCamera);
}

void CShader::ReleaseShaderVariables()
{

}

void CShader::UpdateMainPassCB(const CTimer& timer, CCamera* pCamera)
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
	mMainPassCB.TotalTime = timer.TotalTime();
	mMainPassCB.DeltaTime = timer.DeltaTime();

	auto currPassCB = m_CurrFrameResource->PassCB.get();
	currPassCB->CopyData(0, mMainPassCB);
}

void CShader::UpdateObjectCBs(const CTimer & timer)
{
	auto currObjectCB = m_CurrFrameResource->ObjectCB.get();

	for (auto& object : m_vObjects)
	{
		if (object == nullptr)
			continue;

		RenderItem* pRenderItem = (RenderItem*)object->Get_Component(L"RenderItem");
		if (pRenderItem == nullptr)
			continue;
		// Only update the cbuffer data if the constants have changed.  
		// This needs to be tracked per frame resource.
		if (pRenderItem->NumFramesDirty > 0)
		{
			XMMATRIX world = XMLoadFloat4x4(&pRenderItem->m_Transform.Get_World());
			ObjectConstants objConstants;
			XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(world));

			currObjectCB->CopyData(pRenderItem->ObjCBIndex, objConstants);

			// Next FrameResource need to be updated too.
			pRenderItem->NumFramesDirty--;
		}
	}
}

void CShader::BuildObjects(void)
{

}

void CShader::OnPrepareRender(ID3D12GraphicsCommandList * pd3dCommandList)
{

	ID3D12DescriptorHeap* descriptorHeaps[] = { m_CbvHeap.Get() };
	pd3dCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	pd3dCommandList->SetGraphicsRootSignature(m_RootSignature->Get());

	int passCbvIndex = mPassCbvOffset + m_CurrFrameResourceIndex;
	auto passCbvHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(m_CbvHeap->GetGPUDescriptorHandleForHeapStart());
	passCbvHandle.Offset(passCbvIndex, g_CbvSrvUavDescriptorSize);
	pd3dCommandList->SetGraphicsRootDescriptorTable(1, passCbvHandle);


	if (m_RootSignature[PSO_OBJECT])
		pd3dCommandList->SetGraphicsRootSignature(m_RootSignature[PSO_OBJECT].Get());

	if (m_pPSOs[PSO_OBJECT])
		pd3dCommandList->SetPipelineState(m_pPSOs[PSO_OBJECT].Get());

	pd3dCommandList->SetDescriptorHeaps(1, m_CbvHeap.GetAddressOf());
}

void CShader::Render(ID3D12GraphicsCommandList * pd3dCommandList, CCamera * pCamera)
{
	OnPrepareRender(pd3dCommandList);
	for (auto& object : m_vObjects)
	{
		if (object == nullptr)
			continue;
		object->Render(pd3dCommandList);
	}

	++m_CurrFrameResource->Fence;
}

CObjectsShader::CObjectsShader()
{
}

CObjectsShader::~CObjectsShader()
{
}

void CObjectsShader::BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, XMFLOAT3 Position)
{

}

void CObjectsShader::BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, XMFLOAT3 Position, int Object_Kind)
{
	/*if (Object_Kind == OBJECT_MAP) {
		m_nObjects = 1;
		m_ppObjects = vector<CGameObject*>(m_nObjects);

		CGameObject *Map_Object = NULL;

		Map_Object = new CGameObject();
		setPos(Position);
		Map_Object->SetPosition(Pos_act.x, Pos_act.y, Pos_act.z);
		m_ppObjects[0] = Map_Object;

		CMesh *pCubeMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList, 2000, 20, 1500);
		m_ppObjects[0]->SetMesh(pCubeMesh);

		CreateShaderVariables(pd3dDevice, pd3dCommandList);
	}*/
}

void CObjectsShader::AnimateObjects(float fTimeElapsed)
{
	for (int j = 0; j < m_nObjects; j++)
	{
		//m_ppObjects[j]->Animate(fTimeElapsed);
	}
}

void CObjectsShader::ReleaseObjects()
{
	/*if (m_pMaterial) {
		m_pMaterial->ReleaseShaderVariables();
		m_pMaterial->ReleaseUploadBuffers();
		m_pMaterial->Release();
	}
	for (UINT j = 0; j < m_bbObjects.size(); j++)
	{
		if (m_bbObjects[j]) {
			delete m_bbObjects[j];
		}

	}
	m_bbObjects.clear();*/
}

D3D12_INPUT_LAYOUT_DESC CObjectsShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];
	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;
	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CObjectsShader::CreateVertexShader(ID3DBlob ** ppd3dShaderBlob)
{
	wchar_t filename[100] = L"Shaders.hlsl";
	return(CShader::CompileShaderFromFile(filename, "VSDiffused", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CObjectsShader::CreatePixelShader(ID3DBlob ** ppd3dShaderBlob)
{
	wchar_t filename[100] = L"Shaders.hlsl";
	return(CShader::CompileShaderFromFile(filename, "PSDiffused", "ps_5_1", ppd3dShaderBlob));
}

void CObjectsShader::CreateShader(ID3D12Device * pd3dDevice, ID3D12RootSignature * pd3dGraphicsRootSignature)
{
	m_nPipelineStates = 1;
	m_ppd3dPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];
	CShader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
}

void CObjectsShader::ReleaseUploadBuffers()
{
	
}

void CObjectsShader::Render(UINT frameResourceIndex, ID3D12GraphicsCommandList * pd3dCommandList, CCamera * pCamera)
{
	CShader::Render(pd3dCommandList, pCamera);
	for (int j = 0; j < m_nObjects; j++) { if (m_ppObjects[j]) { m_ppObjects[j]->Render(pd3dCommandList); } }
}
