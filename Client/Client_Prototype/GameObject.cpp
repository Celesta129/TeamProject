
#include "GameObject.h"


CGameObject::CGameObject()
{
}


CGameObject::~CGameObject()
{
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

XMFLOAT4X4* CGameObject::GetWorld(void)
{
	return &m_RenderItem.World;
}

XMFLOAT3 CGameObject::GetPos(void)
{
	return XMFLOAT3(m_RenderItem.World.m[3][0], m_RenderItem.World.m[3][1], m_RenderItem.World.m[3][2]);
}

XMFLOAT3 CGameObject::GetScale(void)
{
	// Scale은 완성 안됐다.
	return XMFLOAT3(m_RenderItem.World.m[3][0], m_RenderItem.World.m[3][1], m_RenderItem.World.m[3][2]);
}


RenderItem* CGameObject::GetRenderItem(void)
{
	return &m_RenderItem;
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
