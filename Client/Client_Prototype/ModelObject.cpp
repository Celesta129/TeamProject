#include "ModelObject.h"


CModelObject::CModelObject()
{
}

CModelObject::~CModelObject()
{
	
}

HRESULT CModelObject::Initialize(void)
{
	HRESULT hr = E_FAIL;
	if(FAILED(hr = CGameObject::Initialize()))
		return hr;
	if(FAILED(hr = Insert_Component_ToMap(L"Component_Model_Floor")))
		return hr;

	return S_OK;
}

HRESULT CModelObject::Initialize(const wstring& tag, ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
	HRESULT hr = E_FAIL;
	if (FAILED(hr = CGameObject::Initialize()))
		return hr;
	if (FAILED(hr = Insert_Component_ToMap(tag)))
		return hr;

	m_pModel.push_back((LoadModel*)Get_Component(tag));

	// m_ani = nullptr;
	m_NumofAnim = 0;
	m_AnimIndex = 0;
	m_Animtime = 0.0f;
	m_nMeshes = m_pModel[m_AnimIndex]->getNumMesh();

	//boundingbox = m_model->getBoudingBox();

	//매쉬 적용
	for (UINT i = 0; i < m_nMeshes; ++i) {
		if (i > 0)
			m_pModel[m_AnimIndex]->SetTextureIndex(i, i);
	}

	//뼈 정보 초기화
	m_Bones.resize(m_pModel[m_AnimIndex]->GetBones()->size());
	//cout << "!! : " << m_model->GetBones()->size() << endl;
	for (auto& p : m_Bones) {
		XMStoreFloat4x4(&p, XMMatrixIdentity());
	}

	return S_OK;
}

void CModelObject::Update(float fTimeElapsed)
{
	CGameObject::Update(fTimeElapsed);
	Animate(fTimeElapsed);
}

void CModelObject::Render(ID3D12GraphicsCommandList * pCommandList)
{
	CGameObject::Render(pCommandList);

	if (m_pModel[m_AnimIndex] != nullptr)
	{
		m_pModel[m_AnimIndex]->Render(pCommandList);
	}

}

SkinnedConstants CModelObject::GetSkinnedConstants(void)
{
	SkinnedConstants result = SkinnedConstants();
	if (m_pModel[m_AnimIndex]) {
		if (m_pModel[m_AnimIndex]->getNumBones() > 0)
		{
			copy(begin(m_pModel[m_AnimIndex]->getBonesTransform()),
				end(m_pModel[m_AnimIndex]->getBonesTransform()),
				&result.BoneTransforms[0]);
		}
		else
		{

		}
	}
	return result;
}

void CModelObject::AddModel(LoadModel * pModel)
{
	if (!pModel)
	{
		m_pModel.push_back(pModel);
	}
}

void CModelObject::Animate(const float fTimeElapsed)
{
	if (m_pModel[m_AnimIndex]->HasAnimation())
	{
		if (m_Animtime > 3.f)
		{
			UINT animIndex = (m_pModel[m_AnimIndex]->getCurrAnimIndex() + 1) % m_pModel[m_AnimIndex]->getNumAnimations();
			if (animIndex != m_pModel[m_AnimIndex]->getCurrAnimIndex())
				m_pModel[m_AnimIndex]->SetCurrAnimIndex(animIndex);
		}
		m_pModel[m_AnimIndex]->BornTransform(fTimeElapsed);
		DirtyFrames();
	}
	m_Animtime += fTimeElapsed;
}
