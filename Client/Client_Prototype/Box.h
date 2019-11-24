#pragma once
#include "Object.h"

class CBox : public CObject
{
public:
	CBox();
	virtual ~CBox();

public:
	virtual void Update(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pCommandList);

};

