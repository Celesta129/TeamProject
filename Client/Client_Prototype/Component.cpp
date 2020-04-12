#include "Component.h"



CComponent::CComponent()
{
}


CComponent::~CComponent()
{
}

UINT CComponent::release(void)
{
	if (--m_nReference <= 0) delete this;
	return m_nReference;
}
