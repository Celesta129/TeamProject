#pragma once
#include "Shader.h"

#define ARRAY_SIZE 6


class CObject_TextureShader : public CShader
{
public:
	CObject_TextureShader();
	~CObject_TextureShader();

public:
	virtual void Update(const CTimer& timer, ID3D12Fence* pFence, CCamera* pCamera);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, UINT64 nFenceValue);
	virtual void Initialize(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pd3dCommandList, const WCHAR* pszShaderFileName);
	void setMat(CGameObject* pObject, int matindex);
protected:
	void LoadTextures(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();
	void CreateMaterial(void);
	void UpdateMaterialCB(void);
protected:
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
	virtual D3D12_BLEND_DESC CreateBlendState();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();

	virtual void CreatePSO(ID3D12Device * pd3dDevice, UINT nRenderTargets, int index);
	virtual void CreateFrameResources(ID3D12Device * pd3dDevice);
	virtual void CreateDescriptorHeaps(ID3D12Device* pd3dDevice);
	virtual void CreateRootSignature(ID3D12Device* pd3dDevice);
	virtual void CreateShader(ID3D12Device *pd3dDevice, ID3D12RootSignature
		*pd3dGraphicsRootSignature);
	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
		*pd3dCommandList);
	virtual void CreateConstantBufferViews(ID3D12Device* pDevice);

	virtual void UpdateShaderVariables(const CTimer& timer, CCamera* pCamera);
	virtual void ReleaseShaderVariables();

	virtual void UpdateMainPassCB(CCamera* pCamera);
	virtual void UpdateObjectCBs();
	virtual void UpdateSkinnedCBs();

	virtual void BuildObjects(void);

	virtual void OnPrepareRender(ID3D12GraphicsCommandList *pd3dCommandList);
private:
	enum PSO_INDEX {
		PSO_OBJECT,
		PSO_END
	};

	enum ROOT_PARAMETER_INDEX {
		RPI_PassConstant,
		RPI_ObjectConstant,
		RPI_MaterialConstant,
		RPI_Texture
	};

private:
	unordered_map<string, unique_ptr<Texture>> m_mapTexture;
	vector<Material*> m_vMaterial;
	int m_iMaterialCbvOffset;
};

