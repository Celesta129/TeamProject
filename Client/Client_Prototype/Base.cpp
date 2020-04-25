#include "Base.h"



CBase::CBase()
{
}


CBase::~CBase()
{
}

int CBase::AddRef()
{
	return ++m_nReference;
}

int CBase::Release()
{
	int result = --m_nReference;

	if (result <= 0) {
		Free();
		delete this;
	} 
	return result;
}
