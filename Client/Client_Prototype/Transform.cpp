#include "stdafx.h"
#include "Transform.h"



CTransform::CTransform()
{
}

CTransform::CTransform(const CTransform & rhs)
{
	m_xmf4x4World = rhs.m_xmf4x4World;
	m_xmf4x4Parent = rhs.m_xmf4x4Parent;
}


CTransform::~CTransform()
{
}

XMFLOAT4X4 CTransform::Get_World(void)
{
	if (m_xmf4x4Parent)
		return Matrix4x4::Multiply(m_xmf4x4World, *m_xmf4x4Parent);

	return m_xmf4x4World;	
}

void CTransform::Set_Parent(XMFLOAT4X4 * pParent)
{
	m_xmf4x4Parent = pParent;
}

void CTransform::Set_World(XMFLOAT4X4 * pWorld)
{
	m_xmf4x4World = *pWorld;
}

const XMFLOAT3 CTransform::Get_Scale(void)
{
	XMFLOAT3 vRight, vUp, vLook;
	memcpy(&vRight, &m_xmf4x4World.m[INFO_RIGHT][0], sizeof(XMFLOAT3));
	memcpy(&vUp, &m_xmf4x4World.m[INFO_UP][0], sizeof(XMFLOAT3));
	memcpy(&vLook, &m_xmf4x4World.m[INFO_LOOK][0], sizeof(XMFLOAT3));

	float lengthRight = Vector3::Length(vRight);
	float lengthUp = Vector3::Length(vUp);
	float lengthLook = Vector3::Length(vLook);

	return XMFLOAT3(lengthRight, lengthUp, lengthLook);
}

const XMFLOAT3 CTransform::Get_Pos(void)
{
	return XMFLOAT3(m_xmf4x4World.m[3]);
}

const XMFLOAT3 CTransform::Get_Dir(void)
{
	XMFLOAT3 Direction;
	Direction.x = Vector3::Length((XMFLOAT3)m_xmf4x4World.m[0]);
	Direction.y = Vector3::Length((XMFLOAT3)m_xmf4x4World.m[1]);
	Direction.z = Vector3::Length((XMFLOAT3)m_xmf4x4World.m[2]);

	Direction = Vector3::Normalize(Direction);
	return Direction;
}

void CTransform::Set_Scale(const XMFLOAT3& pScale)
{
	XMFLOAT3 vRight, vUp, vLook;
	memcpy(&vRight, &m_xmf4x4World.m[INFO_RIGHT][0], sizeof(XMFLOAT3));
	memcpy(&vUp, &m_xmf4x4World.m[INFO_UP][0], sizeof(XMFLOAT3));
	memcpy(&vLook, &m_xmf4x4World.m[INFO_LOOK][0], sizeof(XMFLOAT3));

	vRight = Vector3::Normalize(vRight);
	vUp = Vector3::Normalize(vUp);
	vLook = Vector3::Normalize(vLook);

	vRight = Vector3::Muliply(vRight, pScale.x);
	vUp = Vector3::Muliply(vUp, pScale.y);
	vLook = Vector3::Muliply(vLook, pScale.z);

	memcpy(&m_xmf4x4World.m[INFO_RIGHT][0], &vRight, sizeof(XMFLOAT3));
	memcpy(&m_xmf4x4World.m[INFO_UP][0], &vUp, sizeof(XMFLOAT3));
	memcpy(&m_xmf4x4World.m[INFO_LOOK][0], &vLook, sizeof(XMFLOAT3));
}

void CTransform::Rotate(float fRoll, float fPitch, float fYaw)
{
	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(fRoll),
		XMConvertToRadians(fPitch), XMConvertToRadians(fYaw));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CTransform::MovePos(const XMFLOAT3& Pos)
{
	m_xmf4x4World.m[3][0] += Pos.x;
	m_xmf4x4World.m[3][1] += Pos.y;
	m_xmf4x4World.m[3][2] += Pos.z;
}

void CTransform::Go_Straight(const float fSpeed)
{
	XMFLOAT3 vDir = Vector3::Normalize((XMFLOAT3)m_xmf4x4World.m[2]);
	XMFLOAT3 vMove = Vector3::Muliply(vDir, fSpeed);

	MovePos(vMove);
}

void CTransform::Set_Pos(const XMFLOAT3& Pos)
{
	memcpy(m_xmf4x4World.m[3], &Pos, sizeof(XMFLOAT3));
}

void CTransform::Set_Rotate(float fRoll, float fPitch ,float fYaw)
{
	XMFLOAT3 vPos;
	memcpy(&vPos, &m_xmf4x4World.m[3][0], sizeof(XMFLOAT3));

	XMFLOAT3 vScale = Get_Scale();

	m_xmf4x4World = Matrix4x4::Identity();
	MovePos(vPos);
	Set_Scale(vScale);
	Rotate(fRoll, fPitch, fYaw);
}

void CTransform::Set_Look(XMFLOAT3 & xmf3Look)
{
	if (xmf3Look.x == 0 && xmf3Look.y == 0 && xmf3Look.z == 0)
		return;
	//memcpy(m_xmf4x4World.m[2], &xmf3Look, sizeof(XMFLOAT3));
	XMFLOAT3 vRight, vUp, vLook;
	memcpy(&vUp, m_xmf4x4World.m[INFO_UP], sizeof(XMFLOAT3));
	vUp = Vector3::Normalize(vUp);

	vLook = xmf3Look;
	vLook.y = 0.0f;
	vLook = Vector3::Normalize(vLook);

	vRight = Vector3::CrossProduct(vUp, vLook, true);
	vUp = Vector3::CrossProduct(vLook, vRight, true);

	XMFLOAT3 vScale = Get_Scale();
	vRight = Vector3::Muliply(vRight, vScale.x);
	vUp = Vector3::Muliply(vUp, vScale.y);
	vLook = Vector3::Muliply(vLook, vScale.z);

	memcpy(m_xmf4x4World.m[INFO_RIGHT], &vRight, sizeof(XMFLOAT3));
	memcpy(m_xmf4x4World.m[INFO_UP], &vUp, sizeof(XMFLOAT3));
	memcpy(m_xmf4x4World.m[INFO_LOOK], &vLook, sizeof(XMFLOAT3));
}

CComponent * CTransform::Clone(void)
{
	// 새로 만들어서 넘긴다.
	CTransform* pTransfrom = new CTransform(*this);
	
	return pTransfrom;
}
