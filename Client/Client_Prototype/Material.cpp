#include "Material.h"



CMaterial::CMaterial(const wstring& filename, const wstring & name, ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
	m_pTexture = new CTexture;

	Name = name;
	Filename = filename;
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(pd3dDevice,
		pd3dCommandList, Filename.c_str(),
		m_pTexture->m_pd3dTextures, m_pTexture->m_pd3dTextureUploadBuffers));

	
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = m_pTexture->m_pd3dTextures->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = m_pTexture->m_pd3dTextures->GetDesc().MipLevels;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

	m_pTexture->m_srvDesc = srvDesc;
}


CMaterial::~CMaterial()
{
}


MaterialConstants CMaterial::getMaterialConst(void)
{
	MaterialConstants result;
	result.DiffuseAlbedo = DiffuseAlbedo;
	result.FresnelR0 = FresnelR0;
	result.MatTransform = MatTransform;
	result.Roughness = Roughness;

	return result;
}

CComponent * CMaterial::Clone(void)
{
	AddRef();
	return this;
}

int CMaterial::Free(void)
{
	return 0;
}

CTexture::CTexture()
{
}

CTexture::~CTexture()
{
}

void CTexture::UpdateShaderVariables(ID3D12GraphicsCommandList * pd3dCommandList)
{
}
