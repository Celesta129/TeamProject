
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

void CObject::Render(void)
{

}

XMVECTOR CObject::GetPos(void)
{
	XMVECTOR result = XMVectorSet(m_Pos.x, m_Pos.y, m_Pos.z, 0.f);
	return result;
}
