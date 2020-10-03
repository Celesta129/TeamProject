#pragma once
#include "UIObject.h"
class CUI_WinLose :
	public CUI_Object
{
public:
	CUI_WinLose();
	virtual ~CUI_WinLose();
public:

	virtual HRESULT Initialize(void);
	virtual int Update(float fTimerElapsed);
private:
	float m_fTimer = 5.f;
};

