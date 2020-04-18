
#include "GameObject.h"
#include "Component_Manager.h"

CGameObject::CGameObject()
{
	
}


CGameObject::~CGameObject()
{
	for (auto& e : m_mapComponent)
	{
		e.second->release();
	}
	m_mapComponent.clear();
}

HRESULT CGameObject::Initialize(void)
{
	// ������Ʈ�Ŵ����� �����͸� ȹ��
	m_pComponent_Manager = CComponent_Manager::GetInstance();

	RenderItem* pRenderItem = (RenderItem*)m_pComponent_Manager->Clone_Component(L"RenderItem");
	if (pRenderItem == nullptr)
	{
		_MSG_BOX("RenderItem�� �Ŵ����� ����")
		return E_FAIL;
	}
	m_mapComponent.insert(make_pair(L"RenderItem", pRenderItem));
	m_model = new LoadModel("./resources/idle_Anim.FBX");
	return S_OK;
}

void CGameObject::Update(float fTimeElapsed)
{

}

void CGameObject::Render(ID3D12GraphicsCommandList* pCommandList)
{

}

CComponent * CGameObject::Get_Component(const wstring tag)
{
	auto iter = find_if(m_mapComponent.begin(), m_mapComponent.end(), Finder_Tag(tag));
	if (iter == m_mapComponent.end())
		return nullptr;
	return iter->second;
}

RenderItem* CGameObject::GetRenderItem(void)
{
	return (RenderItem*)Get_Component(L"RenderItem");
}

UINT CGameObject::GetObjCBIndex(void)
{
	return m_RenderItem.ObjCBIndex;
}

UINT CGameObject::GetStartIndexLocation(void)
{
	return m_RenderItem.StartIndexLocation;
}

UINT CGameObject::GetBaseVertexLocation(void)
{
	return m_RenderItem.BaseVertexLocation;
}

void CGameObject::SetMesh(CMesh * pMesh)
{
	//m_RenderItem.Mesh = pMesh;
}
