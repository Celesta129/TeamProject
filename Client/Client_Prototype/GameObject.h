#pragma once
#include "stdafx.h"
#include "../Common/UploadBuffer.h"

#include "RenderItem.h"
#include "LoadModel.h"
class CComponent_Manager;
class CMesh;
class CGameObject
{
public:
	CGameObject();
	virtual ~CGameObject();

public:
	HRESULT Initialize(void);

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
	LoadModel* m_model;

	map<wstring, CComponent*> m_mapComponent;
	RenderItem m_RenderItem;

	CComponent_Manager* m_pComponent_Manager = nullptr;
};

