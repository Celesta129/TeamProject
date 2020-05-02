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

	Animate(fTimeElapsed);
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

void CModel_TextureObject::Animate(const float fTimeElapsed)
{
	if (m_model->HasAnimation()) 
	{
		if (fTime > 3.f)
		{
			UINT animIndex = (m_model->getCurrAnimIndex() + 1) % m_model->getNumAnimations();
			if(animIndex != m_model->getCurrAnimIndex())
				m_model->SetCurrAnimIndex(animIndex);
		}
		m_model->BornTransform(fTimeElapsed);
		DirtyFrames();
	}
	fTime += fTimeElapsed;
}
