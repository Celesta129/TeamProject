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

	m_model = (LoadModel*)Get_Component(tag);

	// m_ani = nullptr;
	m_NumofAnim = 0;
	m_AnimIndex = 0;
	m_Animtime = 0.0f;
	m_nMeshes = m_model->getNumMesh();

	//boundingbox = m_model->getBoudingBox();
	//매쉬 적용
	if (m_nMeshes > 0)
	{
		m_ppMeshes = vector<unique_ptr<CMesh>>(m_nMeshes);
		for (UINT i = 0; i < m_nMeshes; i++)
			m_ppMeshes[i] = nullptr;
	}
	//m_model->SetMeshes(pd3dDevice, pd3dCommandList);

	for (UINT i = 0; i < m_nMeshes; ++i) {
		if (i > 0)
			m_model->SetTextureIndex(i, i);
		SetMesh(i, m_model->getMeshes()[i]);
	}

	//뼈 정보 초기화
	m_Bones.resize(m_model->GetBones()->size());
	//cout << "!! : " << m_model->GetBones()->size() << endl;
	for (auto& p : m_Bones) {
		XMStoreFloat4x4(&p, XMMatrixIdentity());
	}

	return S_OK;
}

void CModelObject::Update(float fTimeElapsed)
{
	CGameObject::Update(fTimeElapsed);
}

void CModelObject::Render(ID3D12GraphicsCommandList * pCommandList)
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

void CModelObject::SetMesh(int nIndex, const shared_ptr<CMesh>& pMesh)
{
	if (!m_ppMeshes.empty())
	{
		
		if (m_ppMeshes[nIndex]) m_ppMeshes[nIndex].release();
		m_ppMeshes[nIndex] = make_unique<CMesh>();
		// 진짜 너네 이런식으로 안하고 make_unique<CMesh>(*pMesh) 어떻게했냐? 복사생성자를 아예 삭제해버리는데?
		
		m_ppMeshes[nIndex]->CopyMesh(pMesh);
	}
}
