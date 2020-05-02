#pragma once
#include "stdafx.h"
#include "Base.h"
#include "ModelObject.h"
#include "FrameResource.h"
#include "Camera.h"

class CTimer;
class CCamera;
class CModelObject;

class CShader : public CBase
{
public:
	CShader();
	virtual ~CShader();
public:
	virtual void Initialize(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pd3dCommandList, const WCHAR* pszShaderFileName) = 0;

			void ResetCmd(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void Update(const CTimer& timer, ID3D12Fence* pFence, CCamera* pCamera);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, UINT64 nFenceValue);

	bool Push_Object(CModelObject* pObject);


	virtual int Free(void);
protected:
	void CreatePipeLineParts(UINT nPSO);
	// ----아래 함수들의 정의는 예제코드이다. 상속받은 Shader 클래스에서 정의하자.---------
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
	virtual D3D12_BLEND_DESC CreateBlendState();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob, const string& shaderName, const D3D_SHADER_MACRO* defines = NULL);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob, const string& shaderName, const D3D_SHADER_MACRO* defines = NULL);
	// ----------------------------------------------------------------------------------
	D3D12_SHADER_BYTECODE CompileShaderFromFile(WCHAR *pszFileName, LPCSTR pszShaderName,
		LPCSTR pszShaderProfile, ID3DBlob **ppd3dShaderBlob, const D3D_SHADER_MACRO* defines = NULL);

protected:
	virtual void CreatePSO(ID3D12Device * pd3dDevice, UINT nRenderTargets, int index);
	virtual void CreateFrameResources(ID3D12Device * pd3dDevice);
	virtual void CreateDescriptorHeaps(ID3D12Device* pd3dDevice);
	virtual void CreateRootSignature(ID3D12Device* pd3dDevice);
	virtual void CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature	*pd3dGraphicsRootSignature);
	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList	*pd3dCommandList);
	virtual void CreateConstantBufferViews(ID3D12Device* pDevice);

	void Initialize_ShaderFileName(const WCHAR* pszShaderFileName);

	virtual void UpdateShaderVariables(const CTimer& timer,  CCamera* pCamera);
	virtual void ReleaseShaderVariables();

	virtual void UpdateMainPassCB(CCamera* pCamera);
	virtual void UpdateObjectCBs();

	virtual void BuildObjects(void);

	virtual void OnPrepareRender(ID3D12GraphicsCommandList *pd3dCommandList);
private:
	enum PSO_INDEX {
		PSO_DEFAULT,
		PSO_END
	};

protected:
	vector<CModelObject**>	m_vpObjects;
	UINT m_nObjects = 0;
	
protected:
	ComPtr<ID3D12RootSignature>* m_RootSignature = nullptr;
	ComPtr<ID3D12DescriptorHeap> m_CbvSrvDescriptorHeap = nullptr;

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
