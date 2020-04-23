#pragma once
#include "stdafx.h"
#include "Base.h"
#include "GameObject.h"
#include "FrameResource.h"
#include "Camera.h"

#define PSO_DEFAULT 0
#define PSO_END 1

class CTimer;
class CCamera;

class CShader : public CBase
{
public:
	CShader();
	virtual ~CShader();
public:
	virtual void Update(const CTimer& timer,  ID3D12Fence* pFence, ID3D12GraphicsCommandList * cmdList, CCamera* pCamera);
	bool Push_Object(CGameObject* pObject);

public:
	virtual void CreatePipeLineParts(UINT nPSO);
	// ----�Ʒ� �Լ����� ���Ǵ� �����ڵ��̴�. ��ӹ��� Shader �ڵ忡�� ��������.---------
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
	virtual D3D12_BLEND_DESC CreateBlendState();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);
	// ----------------------------------------------------------------------------------
	D3D12_SHADER_BYTECODE CompileShaderFromFile(WCHAR *pszFileName, LPCSTR pszShaderName,
		LPCSTR pszShaderProfile, ID3DBlob **ppd3dShaderBlob);

	virtual void CreatePSO(ID3D12Device * pd3dDevice, UINT nRenderTargets, int index);
	virtual void CreateFrameResources(ID3D12Device * pd3dDevice);
	virtual void CreateDescriptorHeaps(ID3D12Device* pd3dDevice);
	virtual void CreateRootSignature(ID3D12Device* pd3dDevice);
	virtual void CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature	*pd3dGraphicsRootSignature);
	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList	*pd3dCommandList);
	virtual void CreateConstantBufferViews(ID3D12Device* pDevice);

	void Initialize_ShaderFileName(const WCHAR* pszShaderFileName);
	virtual void Initialize(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pd3dCommandList, const WCHAR* pszShaderFileName) = 0;
	
	virtual void UpdateShaderVariables(const CTimer& timer, ID3D12GraphicsCommandList *pd3dCommandList, CCamera* pCamera);
	virtual void ReleaseShaderVariables();
	
	virtual void UpdateMainPassCB(CCamera* pCamera);
	virtual void UpdateObjectCBs(const CTimer& timer);
	
	virtual void BuildObjects(void);
	
	virtual void OnPrepareRender(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, UINT64 nFenceValue);

	void ResetCmd(ID3D12GraphicsCommandList* pd3dCommandList);
protected:
	vector<CGameObject**>	m_vpObjects;
	UINT m_nObjects = 0;
	
protected:
	ComPtr<ID3D12RootSignature>* m_RootSignature = nullptr;
	ComPtr<ID3D12DescriptorHeap> m_CbvHeap = nullptr;
	//ComPtr<ID3D12DescriptorHeap> m_CbvSrvDescriptorHeap = nullptr;

	ComPtr<ID3D12PipelineState>* m_pPSOs;		UINT m_nPSO = 0;

	PassConstants mMainPassCB;
protected:
	DXGI_FORMAT	m_Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	UINT m_nNumRenderTarget = 1;

	vector<unique_ptr<FrameResource>> m_vFrameResources;
	UINT m_CurrFrameResourceIndex = -1;
	FrameResource* m_CurrFrameResource = nullptr;

	UINT mPassCbvOffset = 0;

	ID3D12PipelineState * *m_ppd3dPipelineStates = NULL;
	int m_nPipelineStates = 0;
protected:
	wchar_t m_filename[100] = L"Shaders\\color.hlsl";
};
