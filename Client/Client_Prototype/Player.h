#pragma once
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

	void SetWeapon_grab(bool flag) { weapon_grab = flag; }
	void SetWeaponIndex(int index) { weapon_index = index; }
	void SetWeaponType(int type) { weapon_type = type; }

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

	bool m_connected;

	int weapon_type = -1;
	int weapon_index = -1;
	bool weapon_grab = false;

	CModelObject* m_pObjectInstance = nullptr;
};

