#include "Texture.h"

CTexture::CTexture()
{
}

CTexture::~CTexture()
{
}

HRESULT CTexture::Initialize(ID3D12Device* pd3dDevice,ID3D12GraphicsCommandList* pCommandList, string Name, wstring Filename)
{
	if (m_pTexture != nullptr)
	{
		_MSG_BOX("CTexture::Initialize pTexture´Â nullptr°¡ ¾Æ´Ô");
		return E_FAIL;
	}
	m_pTexture = new Texture();
	m_pTexture->Name = Name;
	m_pTexture->Filename = Filename;

	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(pd3dDevice,
		pCommandList, m_pTexture->Filename.c_str(),
		m_pTexture->Resource, m_pTexture->UploadHeap));

	return S_OK;
}

int CTexture::Free()
{
	return 0;
}
