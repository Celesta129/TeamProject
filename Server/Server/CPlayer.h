#pragma once
class CPlayer
{
protected:
public:
	float m_posX, m_posY, m_posZ;
	float m_velX, m_velY, m_velZ;
	int m_sizeX, m_sizeY, m_sizeZ;

	int m_hp;
	char m_Animation_index;
	char m_weapon_index;

	void SetHP(int hp);
	

	CPlayer();
	~CPlayer();
};

