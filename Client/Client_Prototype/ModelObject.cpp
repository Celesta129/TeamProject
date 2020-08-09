#include "ModelObject.h"
#include "Component_Manager.h"

CModelObject::CModelObject()
{
}

CModelObject::~CModelObject()
{
	
}

HRESULT CModelObject::Initialize(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	HRESULT hr = E_FAIL;
	if(FAILED(hr = CGameObject::Initialize()))
		return hr;

	return S_OK;
}

HRESULT CModelObject::Initialize(const wstring& modeltag, const wstring& texturetag, ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
	HRESULT hr = E_FAIL;
	if (FAILED(hr = CGameObject::Initialize()))
		return hr;
	if (FAILED(hr = Insert_Component_ToMap(modeltag)))
		return hr;
	if (FAILED(hr = Insert_Component_ToMap(texturetag)))
		return hr;
	m_pModel.push_back((LoadModel*)Get_Component(modeltag));
	pMaterial = (CMaterial*)Get_Component(texturetag);

	// m_ani = nullptr;
	
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

int CModelObject::Update(float fTimeElapsed)
{
	int result = 0;

	result = CGameObject::Update(fTimeElapsed);
	
	if(m_bAnimStop != true)
		Animate(fTimeElapsed);
	else {
		if (m_Animtime <= 2.4f) {
			Animate(fTimeElapsed);
		}
	}

	return result;
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

void CModelObject::ChangeAnim(const UINT & index)
{
	if (index < m_pModel.size())
	{
		if (m_AnimIndex != index)
		{

			m_pModel[m_AnimIndex]->SetAnimTime(0);

			setAnimIndex(index);

			m_pModel[m_AnimIndex]->SetAnimTime(0);
			m_Animtime = 0.0f;
		}
	}
}

void CModelObject::setAnimIndex(const UINT & index)
{
	m_AnimIndex = index;
	//cout << "modelindex changed : " << index << endl;
}

UINT CModelObject::getAnimIndex(void)
{
	return m_AnimIndex;
}

void CModelObject::AddModel(const wstring& pModelTag)
{
	LoadModel* pModel = dynamic_cast<LoadModel*>(m_pComponent_Manager->Clone_Component(pModelTag));
	if (pModel != nullptr)
	{
		m_pModel.push_back(pModel);
	}
}

void CModelObject::Animate(const float fTimeElapsed)
{
	if (m_pModel[m_AnimIndex]->HasAnimation())
	{
		m_AnimStatus = m_pModel[m_AnimIndex]->BornTransform(fTimeElapsed);
		DirtyFrames();

		m_Animtime += fTimeElapsed;
	}
}
