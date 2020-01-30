#pragma once

#include "Component.h"

class CTransform :
	public CComponent
{
public:
	CTransform();
	virtual ~CTransform();

	XMFLOAT4X4 Get_World(void) {return m_xmf4x4World;	};
private:
	XMFLOAT4X4 m_xmf4x4World;
};

