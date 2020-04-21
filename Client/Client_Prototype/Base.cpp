#include "Base.h"



CBase::CBase()
{
}


CBase::~CBase()
{
}

int CBase::Release()
{
	if (--m_nReference <= 0) delete this;
	return m_nReference;
}
