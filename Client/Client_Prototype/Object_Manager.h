#pragma once
#include "stdafx.h"
#include "GameObject.h"
class CObject_Manager final : public CBase
{
	_DECLARE_SINGLETON(CObject_Manager)
public:
	explicit CObject_Manager();
	virtual ~CObject_Manager() = default;

public:
	enum layers {
		LAYER_PLAYER, LAYER_OBJECT, LAYER_WEAPON, LAYER_UI, LAYER_END
	};

	void Add_Object(UINT layer, CGameObject* pObject);
	CGameObject* Get_Object(UINT layer, UINT index);
	vector<CGameObject*>* Get_Layer(UINT layer);

private:
	vector<CGameObject*> m_vObject[LAYER_END];

	virtual int Free(void);
};

