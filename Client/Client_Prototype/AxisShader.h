#pragma once
#include "Shader.h"

#define PSO_OBJECT 0
#define PSO_END 1

class CAxisShader :	public CShader
{
public:
	CAxisShader();
	virtual ~CAxisShader();
public:
	virtual void Update(const CTimer& timer, ID3D12Fence* pFence, ID3D12GraphicsCommandList * cmdList, CCamera* pCamera);
public:
	virtual void CreatePipeLineParts(UINT nPSO);
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
	virtual D3D12_BLEND_DESC CreateBlendState();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);



	virtual void CreatePSO(ID3D12Device * pd3dDevice, UINT nRenderTargets, int index);
	virtual void CreateFrameResources(ID3D12Device * pd3dDevice);
	virtual void CreateDescriptorHeaps(ID3D12Device* pd3dDevice);
	virtual void CreateRootSignature(ID3D12Device* pd3dDevice);
	virtual void CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature
		*pd3dGraphicsRootSignature);
	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
		*pd3dCommandList);
	virtual void CreateConstantBufferViews(ID3D12Device* pDevice);
	virtual void Initialize(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pd3dCommandList, const WCHAR* pszShaderFileName);

	virtual void UpdateShaderVariables(const CTimer& timer, ID3D12GraphicsCommandList *pd3dCommandList, CCamera* pCamera);
	virtual void ReleaseShaderVariables();

	virtual void UpdateMainPassCB(CCamera* pCamera);
	virtual void UpdateObjectCBs(const CTimer& timer);

	virtual void BuildObjects(void);

	virtual void OnPrepareRender(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, UINT64 nFenceValue);
};

