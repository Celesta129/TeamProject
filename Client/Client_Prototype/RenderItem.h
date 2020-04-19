#pragma once
#include "stdafx.h"
#include "Component.h"

// Object가 그려지기 위해서 필요한 정보들을 모아놓는다.
#include "Transform.h"

struct RenderItem : public CComponent
{
public:
	RenderItem() = default;

	CTransform m_Transform;
	XMFLOAT4X4 World = MathHelper::Identity4x4();
public:
	void MovePos(float fDeltaX, float fDeltaY, float fDeltaZ);
	void MovePos(XMFLOAT3* fDeltaPos);

	void SetPos(XMFLOAT3* fPos);
	void SetScale(XMFLOAT3* fDeltaScale);
public:
	XMFLOAT3 GetPos();
	XMFLOAT3 GetScale();

	// Dirty flag indicating the object data has changed and we need to update the constant buffer.
	// Because we have an object cbuffer for each FrameResource, we have to apply the
	// update to each FrameResource.  Thus, when we modify obect data we should set 
	// NumFramesDirty = NUM_FRAME_RESOURCE so that each frame resource gets the update.
	int NumFramesDirty = NUM_FRAME_RESOURCE;

	// Index into GPU constant buffer corresponding to the ObjectCB for this render item.
	UINT ObjCBIndex = -1;

	MeshGeometry* Geo = nullptr;

	// Primitive topology.
	D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// DrawIndexedInstanced parameters.
	UINT IndexCount = 0;
	UINT StartIndexLocation = 0;
	int BaseVertexLocation = 0;
	
	virtual CComponent* Clone(void);
};
