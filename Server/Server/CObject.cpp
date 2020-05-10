#include"pch.h"
#include "CObject.h"
#include "CPlayer.h"


CObject::CObject()
{
}


CObject::~CObject()
{
}

void CObject::SetObject(float posX, float posY, float posZ, int type)
{
	m_posX = posX;
	m_posY = posY;
	m_posZ = posZ;

	m_Objecttype = type;

	if (m_Objecttype == Bench) {
		m_sizeX = 300.f;
		m_sizeY = 60.f;
		m_sizeZ = 100.f;
	}
	else if (m_Objecttype == SeeSaw) {
		m_sizeX = 350.f;
		m_sizeY = 105.f;
		m_sizeZ = 112.5f;
	}
	else if (m_Objecttype == HobbyHorse) {
		m_sizeX = 115.f;
		m_sizeY = 85.f;
		m_sizeZ = 50.f;
	}
	else if (m_Objecttype == SandBox) {
		m_radius = 540.f;
	}
}

//bool CObject::CheckCollSphere(CPlayer& trg)
//{
//	Point3D trg_min;
//	Point3D trg_max;
//
//	trg_min.x = trg.m_posX - trg.m_sizeX;
//	trg_min.y = trg.m_posY;
//	trg_min.z = trg.m_posZ - trg.m_sizeZ;
//
//	trg_max.x = trg.m_posX + trg.m_sizeX;
//	trg_max.y = trg.m_posY + trg.m_sizeY;
//	trg_max.z = trg.m_posZ + trg.m_sizeZ;
//
//	float close_x = max(trg_min.x, min(m_posX, trg_max.x));
//	float close_y = max(trg_min.y, min(m_posY, trg_max.y));
//	float close_z = max(trg_min.z, min(m_posZ, trg_max.z));
//
//	float fDistance = sqrt((close_x - m_posX) * (close_x - m_posX) +
//		(close_y - m_posY) * (close_y - m_posY) +
//		(close_z - m_posZ) * (close_z - m_posZ));
//
//	if (fDistance > m_radius)
//		return false;   //충돌안됨
//	else
//		return true;    //충돌
//}
//
//bool CObject::CheckCollBox(CPlayer & trg)
//{
//	Point3D trg_min;
//	Point3D trg_max;
//
//	trg_min.x = trg.m_posX - trg.m_sizeX;
//	trg_min.y = trg.m_posY;
//	trg_min.z = trg.m_posZ - trg.m_sizeZ;
//
//	trg_max.x = trg.m_posX + trg.m_sizeX;
//	trg_max.y = trg.m_posY + trg.m_sizeY;
//	trg_max.z = trg.m_posZ + trg.m_sizeZ;
//
//	float Max_X = m_posX + (m_sizeX / 2);
//	float Max_Y = m_posY + m_sizeY;
//	float Max_Z = m_posZ + (m_sizeZ / 2);
//
//	float Min_X = m_posX - (m_sizeX / 2);
//	float Min_Y = m_posY;
//	float Min_Z = m_posZ - (m_sizeZ / 2);
//
//	if ((trg_min.x <= Max_X && trg_max.x >= Min_X) &&
//		(trg_min.y <= Max_Y && trg_max.y >= Min_Y) &&
//		(trg_min.z <= Max_Z && trg_max.z >= Min_Z))
//	{
//		return true;
//	}
//	else
//	{
//		return false;
//	}
//}

