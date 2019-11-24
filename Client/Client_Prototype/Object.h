#pragma once
#include "stdafx.h"
#include "../Common/UploadBuffer.h"

class CMesh;

class CObject
{
protected:
	CObject();
	virtual ~CObject();

public:
	virtual void Update(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pCommandList);

public:
	XMVECTOR GetPos(void);
	XMVECTOR GetScale(void);
public:
	void SetMesh(CMesh* pMesh) {
		m_pMesh = pMesh;
	};
protected:
	
	XMFLOAT3 m_Pos;
	XMFLOAT3 m_Scale;
	XMFLOAT3 m_Rotate;

	UploadBuffer<ObjectConstants>* m_pConstantBuffer;

	CMesh* m_pMesh = nullptr;
};

