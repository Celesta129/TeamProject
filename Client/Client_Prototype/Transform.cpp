#include "stdafx.h"
#include "Transform.h"



CTransform::CTransform()
{
}

CTransform::CTransform(const CTransform & rhs)
{
	m_xmf4x4World = rhs.m_xmf4x4World;
}


CTransform::~CTransform()
{
}

void CTransform::Set_World(XMFLOAT4X4 * pWorld)
{
	m_xmf4x4World = *pWorld;
}

XMFLOAT3 CTransform::Get_Scale(void)
{
	return XMFLOAT3();
}

XMFLOAT3 CTransform::Get_Pos(void)
{
	return XMFLOAT3(m_xmf4x4World.m[3]);
}

void CTransform::Set_Scale(XMFLOAT3 * pScale)
{
}

void CTransform::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch),
		XMConvertToRadians(fYaw), XMConvertToRadians(fRoll));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CTransform::MovePos(XMFLOAT3 * pPos)
{
	m_xmf4x4World.m[3][0] += pPos->x;
	m_xmf4x4World.m[3][1] += pPos->y;
	m_xmf4x4World.m[3][2] += pPos->z;
}

void CTransform::Set_Pos(XMFLOAT3 * pPos)
{
	memcpy(m_xmf4x4World.m[0], pPos, sizeof(XMFLOAT3));
}

CComponent * CTransform::Clone(void)
{
	// 새로 만들어서 넘긴다.
	CTransform* pTransfrom = new CTransform(*this);
	
	return pTransfrom;
}
