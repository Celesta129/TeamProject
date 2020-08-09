#pragma once
#include "Shader.h"
class CUI_Shader :
	public CShader
{
public:
	CUI_Shader();
	virtual ~CUI_Shader();

	virtual int Update(const CTimer& timer, ID3D12Fence* pFence, CCamera* pCamera);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera, UINT64 nFenceValue);
	virtual void Initialize(ID3D12Device * pDevice, ID3D12GraphicsCommandList * pd3dCommandList, const WCHAR * pszShaderFileName, vector<CGameObject*>& vObjects) override;

	virtual bool Push_Object(CGameObject* pObject, UINT PSOindex);
protected:
	void LoadTextures(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void UpdateMaterialCB(void);
	void UpdateUICB(void);
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

	virtual void BuildObjects(vector<CGameObject*>& vObjects, ID3D12Device* pDevice, ID3D12GraphicsCommandList* pd3dCommandList);

	virtual void OnPrepareRender(ID3D12GraphicsCommandList *pd3dCommandList);
	array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();
private:
	enum PSO_INDEX {
		PSO_Edge,
		PSO_HPBar,
		PSO_ETC,
		PSO_END
	};

	enum ROOT_PARAMETER_INDEX {
		RPI_PassConstant,
		RPI_ObjectConstant,
		RPI_MaterialConstant,
		RPI_Texture
	};

private:
	map<wstring, CD3DX12_CPU_DESCRIPTOR_HANDLE> m_mapTextureCPUSrvHandle;
	map<wstring, CD3DX12_GPU_DESCRIPTOR_HANDLE> m_mapTextureGPUSrvHandle;
	typedef map<wstring, CD3DX12_CPU_DESCRIPTOR_HANDLE>::iterator CPUSRVHANDLEMAP;
	typedef map<wstring, CD3DX12_GPU_DESCRIPTOR_HANDLE>::iterator GPUSRVHANDLEMAP;
	int m_iMaterialCbvOffset;

	vector<CGameObject**> m_vObjectVector[PSO_END];
};

