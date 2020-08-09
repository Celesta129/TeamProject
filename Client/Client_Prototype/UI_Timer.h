#pragma once
#include "UIObject.h"
class CUI_Timer :
	public CUI_Object
{
public:
	CUI_Timer();
	virtual ~CUI_Timer();
public:
	virtual HRESULT Initialize(void);

	void Set_Time(int time);

	virtual UI_Constants Get_UIConstants(void);
private:
	int m_iTime = 0;
};

