#pragma once
#include "stdafx.h"
#include "../Common/UploadBuffer.h"

#include "RenderItem.h"
#include "Component.h"
class CMesh;

class CGameObject
{
public:
	CGameObject();
	virtual ~CGameObject();

public:
	virtual void Update(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pCommandList);

public:
	CComponent* Get_Component(const wstring tag);

public:
	RenderItem* GetRenderItem(void);

	UINT GetObjCBIndex(void);
	UINT GetStartIndexLocation(void);
	UINT GetBaseVertexLocation(void);
public:
	void SetMesh(CMesh* pMesh);
	
protected:
	map<wstring, CComponent*> m_mapComponent;
	RenderItem m_RenderItem;
};

