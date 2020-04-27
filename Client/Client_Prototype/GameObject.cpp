
#include "GameObject.h"
#include "Component_Manager.h"

CGameObject::CGameObject()
{
	// 컴포넌트매니저의 포인터를 획득
	m_pComponent_Manager = CComponent_Manager::GetInstance();
	m_pComponent_Manager->AddRef();
}


CGameObject::~CGameObject()
{
	
}

HRESULT CGameObject::Initialize(void)
{
	// Object는 기본적으로 Transform을 멤버로 갖게 있게 설계했다. 취향껏 바꾸자.
	
	if (FAILED(Insert_Component_ToMap(L"Component_Transform")))
		return E_FAIL;
	
	return S_OK;
}

void CGameObject::Update(float fTimeElapsed)
{

}

void CGameObject::Render(ID3D12GraphicsCommandList* pCommandList)
{
	
}

int CGameObject::Free(void)
{
	Safe_Release(m_pComponent_Manager);

	for (auto& e : m_mapComponent)
	{
		int refCnt = Safe_Release(e.second);
	}
	m_mapComponent.clear();

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
		_MSG_BOX("Target Component가 매니저에 없거나 Clone을 거지같이함")
			return E_FAIL;
	}
	m_mapComponent.insert(make_pair(tag, pComponent));

	return S_OK;
}

