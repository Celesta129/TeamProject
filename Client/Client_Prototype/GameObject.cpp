
#include "GameObject.h"

#include "Component_Manager.h"
#include "Object_Manager.h"
CGameObject::CGameObject()
{
	// ������Ʈ�Ŵ����� �����͸� ȹ��
	m_pComponent_Manager = CComponent_Manager::GetInstance();
	m_pComponent_Manager->AddRef();

	m_pObject_Manager = CObject_Manager::GetInstance();
	m_pObject_Manager->AddRef();
}


CGameObject::~CGameObject()
{
	
}

HRESULT CGameObject::Initialize(void)
{
	// Object�� �⺻������ Transform�� ����� ���� �ְ� �����ߴ�. ���ⲯ �ٲ���.
	
	if (FAILED(Insert_Component_ToMap(L"Component_Transform")))
		return E_FAIL;
	
	m_pTransform = (CTransform*)Get_Component(L"Component_Transform");
	return S_OK;
}

int CGameObject::Update(float fTimeElapsed)
{
	return 0;
}

void CGameObject::Render(ID3D12GraphicsCommandList* pCommandList)
{
	
}

int CGameObject::Free(void)
{
	
	for (auto& e : m_mapComponent)
	{
		int refCnt = Safe_Release(e.second);
	}
	m_mapComponent.clear();

	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pObject_Manager);
	return 0;
}

ObjectConstants CGameObject::GetObjectConstants(void)
{
	ObjectConstants result = ObjectConstants();
	CTransform* pTransform = (CTransform*)Get_Component(L"Component_Transform");
	if (pTransform == nullptr)
		return result;

	XMMATRIX world = XMLoadFloat4x4(&pTransform->Get_World());
	XMStoreFloat4x4(&result.World, XMMatrixTranspose(world));
	XMStoreFloat4x4(&result.TexTransform,XMMatrixIdentity());
	return result;
}

CComponent * CGameObject::Get_Component(const wstring tag)
{
	auto iter = find_if(m_mapComponent.begin(), m_mapComponent.end(), Finder_Tag(tag));
	if (iter == m_mapComponent.end())
		return nullptr;
	return iter->second;
}

HRESULT  CGameObject::Insert_Component_ToMap(const wstring tag)
{
	CComponent* pComponent = nullptr;
	pComponent = m_pComponent_Manager->Clone_Component(tag);
	if (pComponent == nullptr)
	{
		_MSG_BOX("Target Component�� �Ŵ����� ���ų� Clone�� ����������")
			return E_FAIL;
	}
	m_mapComponent.insert(make_pair(tag, pComponent));

	return S_OK;
}

