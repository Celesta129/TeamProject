#pragma once
#include "stdafx.h"
class CBase
{
public:
	CBase();
	virtual ~CBase();
public:
	int AddRef();
	int Release();
	virtual int Free(void) = 0;
private:
	int m_nReference = 0;

};

