#include "Player.h"



CPlayer::CPlayer()
{
	m_posX = 0.f;
	m_posY = 0.f;
	m_posZ = 0.f;

	m_velocityX = 0.f;
	m_velocityY = 0.f;
	m_velocityZ = 0.f;

	m_connected = false;
}


CPlayer::~CPlayer()
{
}

void CPlayer::SetPos(float x, float y, float z)
{
	m_posX = x;
	m_posY = y;
	m_posZ = z;
}

void CPlayer::GetPos(float * x, float * y, float * z)
{
	*x = m_posX;
	*y = m_posY;
	*z = m_posZ;
}

void CPlayer::SetVelocity(float x, float y, float z)
{
	m_velocityX = x;
	m_velocityY = y;
	m_velocityZ = z;
}

void CPlayer::GetVelocity(float * x, float * y, float * z)
{
	*x = m_velocityX;
	*y = m_velocityY;
	*z = m_velocityZ;
}

void CPlayer::SetAnimation_index(int animation_index)
{
	m_animation_index = animation_index;
}

void CPlayer::GetAnimation_index(int *animation_index)
{
	*animation_index = m_animation_index;
}

