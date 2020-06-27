#pragma once
#include "ModelObject.h"

class CPlayer;
class CWeapon : public CModelObject
{
public:
	CWeapon() {};
	~CWeapon() {};

public:
	void set_Player(CPlayer* pPlayer);
	CPlayer* get_Player(void) { return m_pPlayer; }
protected:
	XMFLOAT4X4 m_matParent;
	CPlayer* m_pPlayer = nullptr;
};

class CWeapon_Hammer : public CWeapon
{
public:
	CWeapon_Hammer() {};
	~CWeapon_Hammer() {};

public:
	virtual HRESULT Initialize(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual int Update(float fTimeElapsed);
};