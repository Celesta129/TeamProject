#pragma once
#include "stdafx.h"
#include "../Common/UploadBuffer.h"

#include "RenderItem.h"

class CMesh;

class CObject
{
public:
	CObject();
	virtual ~CObject();

public:
	virtual void Update(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pCommandList);

public:
	XMFLOAT4X4* GetWorld(void);
	XMFLOAT3 GetPos(void);
	XMFLOAT3 GetScale(void);

	RenderItem* GetRenderItem(void);
	UINT GetObjCBIndex(void);
	UINT GetStartIndexLocation(void);
	UINT GetBaseVertexLocation(void);
public:
	void SetMesh(CMesh* pMesh);
	
protected:
	RenderItem m_RenderItem;
};

