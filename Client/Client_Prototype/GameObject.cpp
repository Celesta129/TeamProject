
#include "GameObject.h"


CGameObject::CGameObject()
{
	
}


CGameObject::~CGameObject()
{
	for (auto& e : m_mapComponent)
	{
		if(e.second->m_bIsManagers == false)
			delete e.second;
	}
	m_mapComponent.clear();
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
