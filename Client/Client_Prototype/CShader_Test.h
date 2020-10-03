#pragma once
#include "Shader.h"
class Shader_Test :	public CShader
{
public:
	Shader_Test();
	virtual ~Shader_Test();

	// CShader을(를) 통해 상속됨
	virtual void Initialize(ID3D12Device * pDevice, ID3D12GraphicsCommandList * pd3dCommandList, const WCHAR * pszShaderFileName, vector<CGameObject*>& vObjects) override;

	virtual int Update(const CTimer& timer, ID3D12Fence* pFence, CCamera* pCamera);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, UINT64 nFenceValue);
protected:
	array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
	virtual D3D12_BLEND_DESC CreateBlendState();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();

	virtual void CreatePSO(ID3D12Device * pd3dDevice, UINT nRenderTargets, int index);
	virtual void CreateFrameResources(ID3D12Device * pd3dDevice);
	virtual void CreateDescriptorHeaps(ID3D12Device* pd3dDevice);
	virtual void CreateRootSignature(ID3D12Device* pd3dDevice);
	virtual void CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature	*pd3dGraphicsRootSignature);
	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList	*pd3dCommandList);
	virtual void CreateConstantBufferViews(ID3D12Device* pDevice);

	virtual void UpdateShaderVariables(const CTimer& timer, CCamera* pCamera);
	virtual void ReleaseShaderVariables();

	virtual void UpdateMainPassCB(CCamera* pCamera);
	virtual void UpdateMaterialCB();
	virtual void UpdateObjectCBs();

	virtual void BuildObjects(vector<CGameObject*>& vObjects, ID3D12Device* pDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void OnPrepareRender(ID3D12GraphicsCommandList *pd3dCommandList);

private:
	
	enum PSO_INDEX {
		PSO_OBJECT,
		PSO_END
	};

};

