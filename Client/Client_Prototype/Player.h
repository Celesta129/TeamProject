#pragma once
enum PLAYER_STATE {
	NORMAL,
	ATTACK,
	HITTED,
	DEFENSE,
	STUN
};

class CModelObject;
class CPlayer
{
public:
	CPlayer();
	~CPlayer();

	void SetPos(float x, float y, float z);
	void GetPos(float * x, float * y, float * z);
	void SetVelocity(float x, float y, float z);
	void GetVelocity(float * x, float * y, float * z);
	
	void SetAnimation_index(int animation_index);
	void GetAnimation_index(int *animation_index);

	bool Getconnected() { return m_connected; }
	void Setconnected(bool flag) { m_connected = flag; }

	bool GetWeapon_grab() { return weapon_grab; }
	int GetWeaponIndex() { return weapon_index; }
	int GetWeapontype() { return weapon_type; }
	int Get_HP() { return m_hp; }
	char Get_Status() { return m_status; }

	void SetWeapon_grab(bool flag) { weapon_grab = flag; }
	void SetWeaponIndex(int index) { weapon_index = index; }
	void SetWeaponType(int type) { weapon_type = type; }
	void Set_HP(int hp) { m_hp = hp; }
	void Set_status(char status) { m_status = status; }

	void SetObjectInstance(CModelObject* pObject) {
		m_pObjectInstance = pObject;
	}
	CModelObject* getObject(void) {
		return m_pObjectInstance;
	}

private:
	float m_posX, m_posY, m_posZ;
	float m_velocityX, m_velocityY, m_velocityZ;
	int m_animation_index = 0;

	int m_hp = 100;

	bool m_connected;

	int weapon_type = -1;
	int weapon_index = -1;
	bool weapon_grab = false;
	char m_status = PLAYER_STATE::NORMAL;

	CModelObject* m_pObjectInstance = nullptr;
};

