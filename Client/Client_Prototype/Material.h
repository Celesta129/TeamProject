#pragma once
#include "Component.h"

#define RESOURCE_TEXTURE2D						0x01
#define RESOURCE_TEXTURE2D_ARRAY			0x02	//[]
#define RESOURCE_TEXTURE2DARRAY			0x03
#define RESOURCE_TEXTURE_CUBE				0x04
#define RESOURCE_BUFFER							0x05
#define RESOURCE_TEXTURE2D_SHADOWMAP	0x06
#define RESOURCE_TEXTURE2D_HDR	0x07
#define RESOURCE_BUFFER_FLOAT32			0x08

typedef struct SRVROOTARGUMENTINFO
{
	UINT							m_nRootParameterIndex = 0;
	D3D12_GPU_DESCRIPTOR_HANDLE		m_d3dSrvGpuDescriptorHandle;

} SRVROOTARGUMENTINFO;



struct CTexture
{
	CTexture();
	~CTexture();

	int								m_nSamplers = 0;
	UINT							m_nTexType = 0;

	ComPtr<ID3D12Resource>	m_pd3dTextures = nullptr;
	ComPtr<ID3D12Resource>  m_pd3dTextureUploadBuffers = nullptr;
	SRVROOTARGUMENTINFO		m_RootArgumentInfos;
	UINT					m_pTextureType;
	//D3D12_GPU_DESCRIPTOR_HANDLE		*m_pd3dSamplerGpuDescriptorHandles = NULL;
	D3D12_SHADER_RESOURCE_VIEW_DESC m_srvDesc;

	//void AddTexture(ID3D12Resource* texture, ID3D12Resource* uploadbuffer, UINT textureType = RESOURCE_TEXTURE2D);
	void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	//void UpdateComputeShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	//void UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, int nIndex);
	//void ReleaseShaderVariables();
	//D3D12_GPU_DESCRIPTOR_HANDLE GetDescriptorHandle(int index) { return m_pRootArgumentInfos[index].m_d3dSrvGpuDescriptorHandle; }

	//void LoadTextureFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, const wchar_t *pszFileName, UINT nIndex);
	//ComPtr<ID3D12Resource> CreateTexture(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, UINT nWidth, UINT nHeight, DXGI_FORMAT dxgiFormat, 
	//	D3D12_RESOURCE_FLAGS d3dResourceFlags, D3D12_RESOURCE_STATES d3dResourceStates, D3D12_CLEAR_VALUE *pd3dClearValue, UINT nIndex);

	//int GetTextureCount() { return(m_nTextures); }

	//ComPtr<ID3D12Resource> GetTexture(int nIndex) { return(m_ppd3dTextures[nIndex]); }
	//ComPtr<ID3D12Resource> GetUploadBuffer(int nIndex) { return(m_ppd3dTextureUploadBuffers[nIndex]); };


	//UINT GetTextureType(int index) { return(m_pTextureType[index]); }

	//void ReleaseUploadBuffers();
};

class CMaterial : public CComponent
{
public:
	CMaterial(const wstring& filename, const wstring& name, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual ~CMaterial();

public:
	std::wstring Name;
	std::wstring Filename;

	// Material constant buffer data used for shading.
	DirectX::XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
	float Roughness = .25f;
	DirectX::XMFLOAT4X4 MatTransform = MathHelper::Identity4x4();

	CTexture* m_pTexture;

public:
	//void UpdateShaderVariable(void);
	MaterialConstants getMaterialConst(void);
	virtual CComponent* Clone(void);
	virtual int Free(void);
};
