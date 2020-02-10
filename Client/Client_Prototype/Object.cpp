
#include "Object.h"


CObject::CObject()
{
}


CObject::~CObject()
{
}

void CObject::Update(float fTimeElapsed)
{

}

void CObject::Render(ID3D12GraphicsCommandList* pCommandList)
{

}

XMFLOAT4X4* CObject::GetWorld(void)
{
	return &m_RenderItem.World;
}

XMFLOAT3 CObject::GetPos(void)
{
	return XMFLOAT3(m_RenderItem.World.m[3][0], m_RenderItem.World.m[3][1], m_RenderItem.World.m[3][2]);
}

XMFLOAT3 CObject::GetScale(void)
{
	// Scale은 완성 안됐다.
	return XMFLOAT3(m_RenderItem.World.m[3][0], m_RenderItem.World.m[3][1], m_RenderItem.World.m[3][2]);
}


RenderItem* CObject::GetRenderItem(void)
{
	return &m_RenderItem;
}

UINT CObject::GetObjCBIndex(void)
{
	return m_RenderItem.ObjCBIndex;
}

UINT CObject::GetStartIndexLocation(void)
{
	return m_RenderItem.StartIndexLocation;
}

UINT CObject::GetBaseVertexLocation(void)
{
	return m_RenderItem.BaseVertexLocation;
}

void CObject::SetMesh(CMesh * pMesh)
{
	//m_RenderItem.Mesh = pMesh;
}
