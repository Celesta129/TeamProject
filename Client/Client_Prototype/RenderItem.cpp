#include "RenderItem.h"

RenderItem::RenderItem(const RenderItem & rhs)
{
	m_Transform = rhs.m_Transform;
	World = rhs.World;
}

void RenderItem::MovePos(float fDeltaX, float fDeltaY, float fDeltaZ)
{
	
}

void RenderItem::MovePos(XMFLOAT3* fDeltaPos)
{
	m_Transform.MovePos(fDeltaPos);
}

void RenderItem::SetPos(XMFLOAT3* fPos)
{
	m_Transform.Set_Pos(fPos);
}

void RenderItem::SetScale(XMFLOAT3* fDeltaScale)
{
}

XMFLOAT3 RenderItem::GetPos()
{
	return XMFLOAT3();
}

XMFLOAT3 RenderItem::GetScale()
{
	return XMFLOAT3();
}

CComponent * RenderItem::Clone(void)
{
	RenderItem* pRenderItem = new RenderItem(*this);
	
	return pRenderItem;
}
