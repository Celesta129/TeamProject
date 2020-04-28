#pragma once
#include "stdafx.h"
#include "Base.h"

class CComponent : public CBase
{
public:
	CComponent();
	virtual ~CComponent();

public:
	virtual CComponent* Clone(void) = 0;		// Clone 구현시 new Component(this)와 return this; 구분할것. 즉, 새로 복사-생성해서 넘길것과 자신의 포인터만 넘길 컴포넌트를 구분해야함.

	virtual int Free(void);
};

