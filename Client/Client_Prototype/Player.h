#pragma once
#include "ModelObject.h"
enum PLAYER_STATE {
	NORMAL,
	ATTACK,
	HITTED,
	DEFENSE,
	STUN
};

class CWeapon;
class CPlayer : public CModelObject
{
public:
	CPlayer();
	~CPlayer();

public:
	virtual HRESULT Initialize(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	bool collision_weapon(int* pIndex = nullptr);
	bool collision_flag();


	virtual int Update(float fTimeElapsed);
public:
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
	bool Get_guard() { return m_guard; }

	void SetWeapon_grab(bool flag) { weapon_grab = flag; }
	void SetWeaponIndex(int index) { weapon_index = index; }
	void SetWeaponType(int type) { weapon_type = type; }
	void Set_HP(int hp) { m_hp = hp; }
	void Set_status(char status) { m_status = status; }
	void Set_guard(bool flag) { m_guard = flag;}

	
	XMFLOAT4X4 get_Hand(void);
	int matIndex = 0;
	bool m_connected;
private:
	
	bool set_hand(void);
	XMMATRIX* m_pmatHand = nullptr;

	float m_posX, m_posY, m_posZ;
	float m_velocityX, m_velocityY, m_velocityZ;
	int m_animation_index = 0;

	int m_hp = 100;

	

	int weapon_type = -1;
	int weapon_index = -1;
	bool weapon_grab = false;
	bool m_guard = false;
	char m_status = PLAYER_STATE::NORMAL;

};

