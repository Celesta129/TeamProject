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

	m_look = gmtl::Vec3f(0.f, 0.f, 1.f);

	m_hp = 100;
	m_state = PLAYER_STATE::NORMAL;
	m_Animation_index = ANIM_INDEX::IDLE;
	m_dashed = false;

	m_weapon_type = -1;
	m_weapon_index = -1;
	m_weapon_count = 0;

	m_flag = false;

	m_collide_flag = false;
	m_collide_weapon = false;
	m_hitted = false;
}


CPlayer::~CPlayer()
{
}
