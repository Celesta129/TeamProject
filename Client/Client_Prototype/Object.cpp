
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

XMFLOAT4X4 CObject::GetWorld(void)
{
	return m_World;
}

XMFLOAT3 CObject::GetPos(void)
{
	return XMFLOAT3(m_World.m[3][0], m_World.m[3][1], m_World.m[3][2]);
}

XMFLOAT3 CObject::GetScale(void)
{
	return XMFLOAT3(m_World.m[3][0], m_World.m[3][1], m_World.m[3][2]);
}
