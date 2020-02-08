#include "RenderItem.h"

CComponent * RenderItem::Clone(void)
{
	RenderItem* pRenderItem = new RenderItem(*this);
	pRenderItem->m_bIsManagers = false;

	return pRenderItem;
}
