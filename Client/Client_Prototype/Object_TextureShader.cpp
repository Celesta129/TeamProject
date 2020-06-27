#include "Object_TextureShader.h"
#include "ModelObject.h"
#include "Player.h"
CObject_TextureShader::CObject_TextureShader()
{
}


CObject_TextureShader::~CObject_TextureShader()
{
}

int CObject_TextureShader::Update(const CTimer & timer, ID3D12Fence * pFence, CCamera * pCamera)
{
	CShader::Update(timer, pFence, pCamera);
	UpdateShaderVariables(timer, pCamera);
	
	return 0;
}

D3D12_INPUT_LAYOUT_DESC CObject_TextureShader::CreateInputLayout()
{
	UINT nInputElementDescs = 7;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	//���� ������ ���� �Է� �����̴�. ���� VertexData�� ������ ���ʴ�� pos, normal, tan, tex, bornindex,weights, texnum�̴�.

	pd3dInputElementDescs[0] = { "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	0,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[2] = { "TANGENT",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[3] = { "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,		0,	36, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[4] = { "BORNINDEX",	0, DXGI_FORMAT_R32G32B32A32_UINT,	0,	44, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[5] = { "WEIGHT",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	60, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[6] = { "TEXINDEX",	0, DXGI_FORMAT_R32_UINT,			0,	72, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;
	return(d3dInputLayoutDesc);
}

D3D12_RASTERIZER_DESC CObject_TextureShader::CreateRasterizerState()
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

D3D12_BLEND_DESC CObject_TextureShader::CreateBlendState()
{
	return CD3DX12_BLEND_DESC(D3D12_DEFAULT);
}

D3D12_DEPTH_STENCIL_DESC CObject_TextureShader::CreateDepthStencilState()
{
	return CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
}

void CObject_TextureShader::CreatePSO(ID3D12Device * pd3dDevice, UINT nRenderTargets, int index)
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

	const D3D_SHADER_MACRO skinnedDefines[] =
	{
		"SKINNED", "1",
		NULL, NULL
	};
	psoDesc.VS = CreateVertexShader(&pd3dVertexShaderBlob, "VS", skinnedDefines);
	psoDesc.PS = CreatePixelShader(&pd3dPixelShaderBlob, "PS", skinnedDefines);

	psoDesc.RasterizerState = CreateRasterizerState();
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
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
	UINT nNumDescriptor = 2;
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

	auto bricksTex = m_mapTexture["bricksTex"]->Resource;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = bricksTex->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = bricksTex->GetDesc().MipLevels;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	pd3dDevice->CreateShaderResourceView(bricksTex.Get(), &srvDesc, hDescriptor);

	hDescriptor.Offset(1, g_CbvSrvUavDescriptorSize);

	auto floorTex = m_mapTexture["floorTex"]->Resource;
	
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = floorTex->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = floorTex->GetDesc().MipLevels;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
	pd3dDevice->CreateShaderResourceView(floorTex.Get(), &srvDesc, hDescriptor);
}

void CObject_TextureShader::CreateRootSignature(ID3D12Device * pd3dDevice)
{
	UINT numSlot = 5;

	CD3DX12_DESCRIPTOR_RANGE texTable;
	texTable.Init(
		D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
		1,  // number of descriptors
		0); // register t0

	// Root parameter can be a table, root descriptor or root constants.
	CD3DX12_ROOT_PARAMETER slotRootParameter[5];

	// Perfomance TIP: Order from most frequent to least frequent.
	
	slotRootParameter[0].InitAsConstantBufferView(0); // register b0 : pass
	slotRootParameter[1].InitAsConstantBufferView(1); // register b1 : objectcb
	slotRootParameter[2].InitAsConstantBufferView(2); // register b2 : material
	slotRootParameter[3].InitAsConstantBufferView(3); // register b3 : bornfirnaltransform
	slotRootParameter[4].InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_PIXEL);

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

void CObject_TextureShader::CreateShader(ID3D12Device * pd3dDevice, ID3D12RootSignature * pd3dGraphicsRootSignature)
{
}

void CObject_TextureShader::CreateShaderVariables(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{

}

void CObject_TextureShader::CreateConstantBufferViews(ID3D12Device * pDevice)
{
	
}

void CObject_TextureShader::Initialize(ID3D12Device * pDevice, ID3D12GraphicsCommandList * pd3dCommandList, const WCHAR * pszShaderFileName, vector<CGameObject*>& vObjects)
{
	CShader::Initialize_ShaderFileName(pszShaderFileName);
	LoadTextures(pDevice,pd3dCommandList);
	CreatePipeLineParts(1);
	CreateRootSignature(pDevice);
	CreateDescriptorHeaps(pDevice);
	CreateShaderVariables(pDevice, pd3dCommandList);
	CreateMaterial();
	CreateFrameResources(pDevice);
	CreateConstantBufferViews(pDevice);
	
	CreatePSO(pDevice, 1, PSO_OBJECT);

	BuildObjects(vObjects, pDevice, pd3dCommandList);
}

void CObject_TextureShader::setMat(CModelObject* pObject, int matindex)
{
	pObject->setMat(m_vMaterial[matindex]);
}

void CObject_TextureShader::UpdateShaderVariables(const CTimer & timer, CCamera * pCamera)
{
	UpdateMainPassCB(pCamera);
	auto currObjectCB = m_CurrFrameResource->ObjectCB.get();
	auto currMaterialCB = m_CurrFrameResource->MaterialCB.get();
	auto currSkinnedCB = m_CurrFrameResource->SkinnedCB.get();
	for (UINT i = 0; i < m_vpObjects.size(); ++i)
	{
		CModelObject* pObject = *m_vpObjects[i];
		if (pObject == nullptr)
			continue;

		bool result = true;
		if (pObject->GetFramesDirty() > 0)
		{
			currObjectCB->CopyData(i, pObject->GetObjectConstants());
			currSkinnedCB->CopyData(i, pObject->GetSkinnedConstants());
			
			CMaterial* material = pObject->GetMaterial();
			if (material == nullptr)
				result = false;

			XMMATRIX matTransform = XMLoadFloat4x4(&material->MatTransform);

			MaterialConstants matConstants = material->getMaterialConst();
			currMaterialCB->CopyData(i, matConstants);

		}
		if (result == true)
			pObject->DecreaseFramesDirty();
	}
}

void CObject_TextureShader::ReleaseShaderVariables()
{
}

void CObject_TextureShader::UpdateMainPassCB(CCamera * pCamera)
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

void CObject_TextureShader::UpdateObjectCBs()
{
	auto currObjectCB = m_CurrFrameResource->ObjectCB.get();
	for (UINT index = 0; index < m_vpObjects.size(); ++index)
	{
		CGameObject* pObject = *(m_vpObjects[index]);

		if (pObject == nullptr)
			continue;
		// ObjectCB�� ������ �ʿ䰡 ����ٸ� �����Ѵ�.
		if (pObject->GetFramesDirty() > 0)
		{
			currObjectCB->CopyData(index, pObject->GetObjectConstants());
			pObject->DecreaseFramesDirty();
		}
	}
}

void CObject_TextureShader::UpdateSkinnedCBs()
{
	auto currSkinnedCB = m_CurrFrameResource->SkinnedCB.get();
	for (UINT index = 0; index < m_vpObjects.size(); ++index)
	{
		CModelObject* pObject = dynamic_cast<CModelObject*>(*m_vpObjects[index]);

		if (pObject == nullptr)
			continue;
		// ObjectCB�� ������ �ʿ䰡 ����ٸ� �����Ѵ�.
		if (pObject->GetFramesDirty() > 0)
		{
			currSkinnedCB->CopyData(index, pObject->GetSkinnedConstants());
			pObject->DecreaseFramesDirty();
		}
	}
}

void CObject_TextureShader::BuildObjects(vector<CGameObject*>& vObjects, ID3D12Device* pDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	CModelObject* pObject = nullptr;
	CTransform* pTransform = nullptr;
	
	int MAX_USER = 8;
	vector<CGameObject*>* pvObjects = m_pObject_Manager->Get_Layer(CObject_Manager::LAYER_PLAYER);
	for (int i = 0; i < MAX_USER; ++i)
	{
		pObject = new CPlayer();
		
		pObject->Initialize(pDevice, pd3dCommandList);
		
		pvObjects->push_back(pObject);		// ��ü ������Ʈ ���� ���Ϳ� �ִ´�.
		Push_Object(pObject);

		setMat(pObject, 0);
		
		pTransform = GET_COMPONENT(CTransform*, pObject, L"Component_Transform");
		pTransform->MovePos(XMFLOAT3(3000, 0.f, 3000));
		pTransform->Set_Scale(XMFLOAT3(2.f, 2.f, 2.f));

		
	}

}

void CObject_TextureShader::OnPrepareRender(ID3D12GraphicsCommandList * pd3dCommandList)
{
	ID3D12DescriptorHeap* descriptorHeaps[] = { m_CbvSrvDescriptorHeap.Get() };
	pd3dCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps); //pd3dCommandList->SetDescriptorHeaps(1, m_CbvHeap.GetAddressOf());

	//pd3dCommandList->SetGraphicsRootSignature(m_RootSignature->Get());
	if (m_RootSignature[PSO_OBJECT])		// ��Ʈ������ Set�ϴ� ���� ��� ���ε��� �������. ���⼭���� �� ��Ʈ������ ����ϴ� �ڿ��� ���� �����Ѵ�.
		pd3dCommandList->SetGraphicsRootSignature(m_RootSignature[PSO_OBJECT].Get());

	if (m_pPSOs[PSO_OBJECT])
		pd3dCommandList->SetPipelineState(m_pPSOs[PSO_OBJECT].Get());

	auto passCB = m_CurrFrameResource->PassCB->Resource();
	pd3dCommandList->SetGraphicsRootConstantBufferView(0, passCB->GetGPUVirtualAddress());
	//pd3dCommandList->SetGraphicsRootDescriptorTable(0, passCbvHandle);		// ������ ���̺��� ���������ο� ���´�. // Pass RootDescriptor�� 0��
}

void CObject_TextureShader::Render(ID3D12GraphicsCommandList * pd3dCommandList, CCamera * pCamera, UINT64 nFenceValue)
{
	OnPrepareRender(pd3dCommandList);
	UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));
	UINT matCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(MaterialConstants));
	UINT skinnedCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(SkinnedConstants));
	auto objectCB = m_CurrFrameResource->ObjectCB->Resource();
	auto matCB = m_CurrFrameResource->MaterialCB->Resource();
	auto skinnedCB = m_CurrFrameResource->SkinnedCB->Resource();
	for (UINT index = 0; index < m_vpObjects.size(); ++index)
	{
		CModelObject* pObject = (CModelObject*)*(m_vpObjects[index]);

		if (pObject == nullptr)
			continue;

		CD3DX12_GPU_DESCRIPTOR_HANDLE tex(m_CbvSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
		Material* pMaterial = pObject->getMat();
		tex.Offset(pMaterial->DiffuseSrvHeapIndex, g_CbvSrvUavDescriptorSize);

		D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = objectCB->GetGPUVirtualAddress() + index*objCBByteSize;
		D3D12_GPU_VIRTUAL_ADDRESS matCBAddress = matCB->GetGPUVirtualAddress() + pMaterial->MatCBIndex*matCBByteSize;
		D3D12_GPU_VIRTUAL_ADDRESS skinnedCBAddress = skinnedCB->GetGPUVirtualAddress() + index * skinnedCBByteSize;
		
		pd3dCommandList->SetGraphicsRootConstantBufferView(1, objCBAddress);
		pd3dCommandList->SetGraphicsRootConstantBufferView(2, matCBAddress);
		pd3dCommandList->SetGraphicsRootConstantBufferView(3, skinnedCBAddress);
		pd3dCommandList->SetGraphicsRootDescriptorTable(4, tex);

		pObject->Render(pd3dCommandList);
	}

	m_CurrFrameResource->Fence = nFenceValue;
}

void CObject_TextureShader::LoadTextures(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	auto bricksTex = std::make_unique<Texture>();
	bricksTex->Name = "bricksTex";
	bricksTex->Filename = L"resources/character_test.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(pd3dDevice,
		pd3dCommandList, bricksTex->Filename.c_str(),
		bricksTex->Resource, bricksTex->UploadHeap));

	m_mapTexture[bricksTex->Name] = std::move(bricksTex);

	auto floorTex = std::make_unique<Texture>();
	floorTex->Name = "floorTex";
	floorTex->Filename = L"resources/dds/SandBox_diffuse.dds";
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(pd3dDevice,
		pd3dCommandList, floorTex->Filename.c_str(),
		floorTex->Resource, floorTex->UploadHeap));

	m_mapTexture[floorTex->Name] = std::move(floorTex);
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

void CObject_TextureShader::CreateMaterial(void)
{
	Material* bricks0 = new Material;
	bricks0->Name = "bricks0";
	bricks0->MatCBIndex = 0;
	bricks0->DiffuseSrvHeapIndex = 0;
	bricks0->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	bricks0->FresnelR0 = XMFLOAT3(0.02f, 0.02f, 0.02f);
	bricks0->Roughness = 0.1f;

	m_vMaterial.push_back(move(bricks0));

	Material* Floor = new Material;
	Floor->Name = "Floor";
	Floor->MatCBIndex = 1;
	Floor->DiffuseSrvHeapIndex = 1;
	Floor->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	Floor->FresnelR0 = XMFLOAT3(0.02f, 0.02f, 0.02f);
	Floor->Roughness = 0.1f;

	m_vMaterial.push_back(move(Floor));
}

void CObject_TextureShader::UpdateMaterialCB(void)
{
	auto currMaterialCB = m_CurrFrameResource->MaterialCB.get();
	for (auto& e : m_vMaterial)
	{
		// Only update the cbuffer data if the constants have changed.  If the cbuffer
		// data changes, it needs to be updated for each FrameResource.
		Material* mat = e;
		if (mat->NumFramesDirty > 0)
		{
			XMMATRIX matTransform = XMLoadFloat4x4(&mat->MatTransform);

			MaterialConstants matConstants;
			matConstants.DiffuseAlbedo = mat->DiffuseAlbedo;
			matConstants.FresnelR0 = mat->FresnelR0;
			matConstants.Roughness = mat->Roughness;
			XMStoreFloat4x4(&matConstants.MatTransform, XMMatrixTranspose(matTransform));

			currMaterialCB->CopyData(mat->MatCBIndex, matConstants);

			// Next FrameResource need to be updated too.
			mat->NumFramesDirty--;
		}
	}
}
