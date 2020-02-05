#include "stdafx.h"
#include "Transform.h"



CTransform::CTransform()
{
}


CTransform::~CTransform()
{
}

XMFLOAT3 CTransform::GetPos(void)
{
	return XMFLOAT3(m_xmf4x4World.m[3]);
}

void CTransform::MovePos(XMFLOAT3 * pPos)
{
	m_xmf4x4World.m[3][0] += pPos->x;
	m_xmf4x4World.m[3][1] += pPos->y;
	m_xmf4x4World.m[3][2] += pPos->z;
}

void CTransform::SetPos(XMFLOAT3 * pPos)
{
	memcpy(m_xmf4x4World.m[0], pPos, sizeof(XMFLOAT3));
}
