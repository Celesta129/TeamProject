#pragma once
#include "stdafx.h"
#include "../Common/UploadBuffer.h"

class CObject
{
protected:
	CObject();
	virtual ~CObject();

public:
	virtual void Update(float fTimeElapsed);
	virtual void Render(void);

public:
	XMVECTOR GetPos(void);

protected:
	
	XMFLOAT3 m_Pos;


	XMFLOAT4X4 mWorld = MathHelper::Identity4x4();
	XMFLOAT4X4 mView = MathHelper::Identity4x4();
	XMFLOAT4X4 mProj = MathHelper::Identity4x4();

	float mTheta = 1.5f*XM_PI;
	float mPhi = XM_PIDIV4;
	float mRadius = 5.0f;

	UploadBuffer<ObjectConstants>* m_pConstantBuffer;
};

