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
	XMFLOAT4X4 GetWorld(void);
	XMFLOAT3 GetPos(void);
	XMFLOAT3 GetScale(void);
public:
	void SetMesh(CMesh* pMesh) {
		m_pMesh = pMesh;
	};
protected:
	XMFLOAT4X4 m_World = MathHelper::Identity4x4();

	UINT m_ObjCBIndex = -1;		// 상수 버퍼 인덱스

	CMesh* m_pMesh = nullptr;
};

