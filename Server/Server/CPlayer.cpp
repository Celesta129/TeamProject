#include "pch.h"
#include "CPlayer.h"


void CPlayer::SetHP(int hp)
{
	m_hp = hp;
}

CPlayer::CPlayer()
{
	m_posX = 0.f;
	m_posY = 0.f;
	m_posZ = 0.f;

	m_velX = 0.f;
	m_velY = 0.f;
	m_velZ = 0.f;

	m_sizeX = 27.f;
	m_sizeY = 136.f;
	m_sizeZ = 27.f;

	m_look.x = 0.f;
	m_look.y = 0.f;
	m_look.z = 1.f;

	m_hp = 100;
	m_state = PLAYER_STATE::NORMAL;
	m_Animation_index = 0;
	m_dashed = false;
}


CPlayer::~CPlayer()
{
}
