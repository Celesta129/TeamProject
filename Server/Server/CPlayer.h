#pragma once
enum PLAYER_STATE {
	NORMAL,
	ATTACK,
	HITTED,
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
	GUARD,
	ANI_STUN,
	GETUP,
	WIN,
	LOSE
};

class CPlayer
{
protected:
public:
	float m_posX, m_posY, m_posZ;
	float m_velX, m_velY, m_velZ;
	int m_sizeX, m_sizeY, m_sizeZ;

	gmtl::Vec3f m_look;

	int m_hp;
	char m_state;
	char m_Animation_index;

	char m_weapon_index;
	char m_weapon_type;
	char m_weapon_count;

	char last_hit;
	char flag_time;

	volatile bool m_flag;
	volatile bool m_dashed;
	volatile bool m_guard;

	volatile bool m_hitted;
	volatile bool m_collide_flag;
	volatile bool m_collide_weapon;

	void SetHP(int hp);
	

	CPlayer();
	~CPlayer();
};

