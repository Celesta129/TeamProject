#include "Model_TextureObject.h"



CModel_TextureObject::CModel_TextureObject()
{
}


CModel_TextureObject::~CModel_TextureObject()
{
}

HRESULT CModel_TextureObject::Initialize(void)
{

	return CModelObject::Initialize();
}

HRESULT CModel_TextureObject::Initialize(const wstring & modelTag, ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
	HRESULT hr = CModelObject::Initialize(modelTag, pd3dDevice, pd3dCommandList);

	return hr;
}

void CModel_TextureObject::Update(float fTimeElapsed)
{
	CGameObject::Update(fTimeElapsed);
}

void CModel_TextureObject::Render(ID3D12GraphicsCommandList * pCommandList)
{
	CGameObject::Render(pCommandList);

	if (!m_ppMeshes.empty())
	{
		for (UINT i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i])
				m_ppMeshes[i]->Render(pCommandList);
		}
	}
}
