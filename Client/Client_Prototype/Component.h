#pragma once
#include "stdafx.h"
#include "Base.h"

class CComponent : public CBase
{
public:
	CComponent();
	virtual ~CComponent();

public:
	virtual CComponent* Clone(void) = 0;		// Clone ������ new Component(this)�� return this; �����Ұ�. ��, ���� ����-�����ؼ� �ѱ�Ͱ� �ڽ��� �����͸� �ѱ� ������Ʈ�� �����ؾ���.

	virtual int Free(void);
};

