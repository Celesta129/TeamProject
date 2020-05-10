#include "CPlayer.h"



void CPlayer::SetHP(int hp)
{
	m_hp = hp;
}

CPlayer::CPlayer()
{
	m_Animation_index = 0;
	m_hp = 100;
	m_sizeX = 27;
	m_sizeZ = 27;
	m_sizeY = 136;
}


CPlayer::~CPlayer()
{
}
