#pragma once
#include "UIObject.h"
class CUI_ReadyStart :
	public CUI_Object
{
public:
	CUI_ReadyStart();
	virtual ~CUI_ReadyStart();
public:

	virtual HRESULT Initialize(void);
};

