#pragma once
enum PLAYER_STATE {
	NORMAL,
	DAMAGED,
	DEFENSE,
	STUN
};

enum ANIM_INDEX {
	IDLE,
	RUN,
	BASIC_ATTACK,
	HIT,
	PICK_UP,
	HAMMER_ATTACK,
	SWORD_ATTACK,
	SNACK_ATTACK,
	BLOCK_ATTACK,
	GUARD
};

class CPlayer
{
protected:
public:
	float m_posX, m_posY, m_posZ;
	float m_velX, m_velY, m_velZ;
	int m_sizeX, m_sizeY, m_sizeZ;

	Point3D m_look;

	int m_hp;
	char m_state;
	char m_Animation_index;
	char m_weapon_index;
	volatile bool m_dashed;

	void SetHP(int hp);
	

	CPlayer();
	~CPlayer();
};

