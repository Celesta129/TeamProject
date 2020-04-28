#include "CModel_AnimationObject.h"



CModel_AnimationObject::CModel_AnimationObject()
{
}


CModel_AnimationObject::~CModel_AnimationObject()
{
}

HRESULT CModel_AnimationObject::Initialize(void)
{
	return CModel_TextureObject::Initialize();
}

HRESULT CModel_AnimationObject::Initialize(const wstring & modelTag, ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
	HRESULT hr = CModel_TextureObject::Initialize();



	return hr;
}

void CModel_AnimationObject::Update(float fTimeElapsed)
{
	CModel_TextureObject::Update(fTimeElapsed);
}

void CModel_AnimationObject::Render(ID3D12GraphicsCommandList * pCommandList)
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
