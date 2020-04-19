#pragma once
#include "stdafx.h"
#include "GameObject.h"
#include "FrameResource.h"

#define PSO_OBJECT 0

class CTimer;
class CCamera;

class CShader
{
public:
	CShader();
	virtual ~CShader();
private:
	int m_nReferences = 0;
public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	void Update(const CTimer& timer,  ID3D12Fence* pFence, ID3D12GraphicsCommandList * cmdList);

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
	virtual D3D12_BLEND_DESC CreateBlendState();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CompileShaderFromFile(WCHAR *pszFileName, LPCSTR pszShaderName,
		LPCSTR pszShaderProfile, ID3DBlob **ppd3dShaderBlob);

	virtual void CreatePipeLineParts(void);
	virtual void CreatePSO(ID3D12Device * pd3dDevice, UINT nRenderTargets, int index);
	virtual void CreateFrameResources(ID3D12Device * pd3dDevice);
	virtual void CreateDescriptorHeaps(ID3D12Device* pd3dDevice);
	virtual void CreateRootSignature(ID3D12Device* pd3dDevice);
	virtual void CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature
		*pd3dGraphicsRootSignature);
	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
		*pd3dCommandList);
	virtual void Initialize(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	
	
	virtual void UpdateShaderVariables(const CTimer& timer, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();
	
	virtual void UpdateMainPassCB(const CTimer& timer);
	virtual void UpdateObjectCBs(const CTimer& timer);
	
	virtual void BuildObjects(void);
	
	virtual void OnPrepareRender(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void Render(UINT frameResourceIndex, ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);

protected:
	vector<CGameObject* >	m_vObjects;
	UINT m_nObjects = 0;

	CCamera* m_pCurrentCamera;
protected:
	ComPtr<ID3D12RootSignature>* m_RootSignature = nullptr;
	ComPtr<ID3D12DescriptorHeap> m_CbvHeap = nullptr;
	ComPtr<ID3D12PipelineState>* m_pPSOs;		UINT m_nPSO = 0;

	PassConstants mMainPassCB;
protected:
	DXGI_FORMAT	m_Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	UINT m_nNumRenderTarget = 1;

	vector<unique_ptr<FrameResource>> m_vFrameresources;
	UINT m_CurrFrameResourceIndex = 0;
	FrameResource* m_CurrFrameResource = nullptr;

	UINT mPassCbvOffset = 0;

	ID3D12PipelineState * *m_ppd3dPipelineStates = NULL;
	int m_nPipelineStates = 0;
};

//“CObjectsShader” 클래스는 게임 객체들을 포함하는 셰이더 객체이다.
class CObjectsShader : public CShader
{
public:
	CObjectsShader();
	virtual ~CObjectsShader();
	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, XMFLOAT3 Position);
	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, XMFLOAT3 Position, int Object_Kind);
	virtual void AnimateObjects(float fTimeElapsed);
	virtual void ReleaseObjects();
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);
	virtual void CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature	*pd3dGraphicsRootSignature);
	virtual void ReleaseUploadBuffers();
	virtual void Render(UINT frameResourceIndex, ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
protected:
	
	vector<CGameObject* >	m_ppObjects;
	//vector<ModelObject* >	m_bbObjects;
	//CMaterial							*m_pMaterial = NULL;
	int m_nObjects = 0;
};
class CModelShader : public CObjectsShader
{
public:
public:
	CModelShader();
	//CModelShader(Model_Animation *ma);
	CModelShader(LoadModel *ma);
	CModelShader(UINT index);
	~CModelShader();

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, XMFLOAT3 Position);
	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, XMFLOAT3 Position, int Object_Kind);
	virtual void AnimateObjects(float fTimeElapsed);
	virtual void ReleaseObjects();
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);
	virtual void CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature	*pd3dGraphicsRootSignature);
	virtual void ReleaseUploadBuffers();
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);
private:
	ComPtr<ID3D12RootSignature>*					m_RootSignature = nullptr;
	ComPtr<ID3D12RootSignature>*					m_ComputeRootSignature = nullptr;

	ComPtr<ID3D12DescriptorHeap>					m_CBVHeap = nullptr;
	ComPtr<ID3D12DescriptorHeap>					m_CbvSrvDescriptorHeap = nullptr;
	ComPtr<ID3D12DescriptorHeap>					m_ComputeCbvSrvDescriptorHeap = nullptr;

	ComPtr<ID3D12PipelineState>*					m_pPSO = nullptr;
	ComPtr<ID3D12PipelineState>*					m_pComputePSO = nullptr;


	ComPtr<ID3DBlob>*								m_CSByteCode = nullptr;


	vector<D3D12_INPUT_ELEMENT_DESC>				m_pInputElementDesc;

	UINT											m_nObjects = 0;
	UINT											m_nPSO = 1;
	UINT											m_nComputePSO = 0;
	UINT											m_nComputeBuffers = 0;
	UINT											m_nDescriptorUAVStartIndex = 0;
	XMUINT3*										m_nComputeThreadCount;
	XMUINT2*										m_nBuffTypeElements;

	D3D12_CPU_DESCRIPTOR_HANDLE						m_d3dCbvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE						m_d3dCbvGPUDescriptorStartHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE						m_d3dSrvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE						m_d3dSrvGPUDescriptorStartHandle;

	D3D12_CPU_DESCRIPTOR_HANDLE						m_d3dComputeCbvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE						m_d3dComputeCbvGPUDescriptorStartHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE						m_d3dComputeSrvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE						m_d3dComputeSrvGPUDescriptorStartHandle;

	vector<ComPtr<ID3D12Resource>>					m_pComputeUAVBuffers;
	vector<ComPtr<ID3D12Resource>>					m_pComputeOutputBuffers;

	vector<D3D12_CPU_DESCRIPTOR_HANDLE>				m_pComputeSRVUAVCPUHandles;
	vector<D3D12_GPU_DESCRIPTOR_HANDLE>				m_pComputeSRVUAVGPUHandles;

	vector<D3D12_INPUT_ELEMENT_DESC>				m_InputLayout;

};