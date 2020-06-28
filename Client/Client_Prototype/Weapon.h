#pragma once
#include "ModelObject.h"

class CPlayer;
class CWeapon : public CModelObject
{
public:
	enum enum_type {
		WEAPON_SWORD, WEAPON_HAMMER, WEAPON_SNACK, WEAPON_BLOCK, FLAG
	};
public:
	CWeapon(UINT weapontype = WEAPON_HAMMER) { m_type = weapontype; };
	~CWeapon() {};

public:
	void set_Player(CPlayer* pPlayer);
	CPlayer* get_Player(void) { return m_pPlayer; }

	void set_Type(UINT weapontype) { m_type = weapontype; }

	virtual HRESULT Initialize(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual int Update(float fTimeElapsed);
protected:
	XMFLOAT4X4 m_matParent;
	CPlayer* m_pPlayer = nullptr;

	UINT m_type = 0;
};
