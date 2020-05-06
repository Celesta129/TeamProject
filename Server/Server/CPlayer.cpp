#include "CPlayer.h"



void CPlayer::SetHP(int hp)
{
	m_hp = hp;
}

CPlayer::CPlayer()
{
	m_Animation_index = 0;
	m_hp = 100;
	m_sizeX = 50;
	m_sizeZ = 50;
	m_sizeY = 150;
}


CPlayer::~CPlayer()
{
}
