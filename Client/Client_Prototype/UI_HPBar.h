#pragma once
#include "UIObject.h"
class CUI_HPBar :
	public CUI_Object
{
public:
	CUI_HPBar();
	virtual ~CUI_HPBar();
public:
	virtual HRESULT Initialize(void);

	virtual int Update(float fTimeElapsed);
	//virtual void Render(ID3D12GraphicsCommandList* pCommandList);

	virtual UI_Constants Get_UIConstants(void);

public:
	virtual void SetTarget(CGameObject* pTarget);
private:
	CPlayer* m_pTarget = nullptr;
	float m_fHP = 0.f;
};

