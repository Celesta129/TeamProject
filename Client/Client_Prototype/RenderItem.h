#pragma once
#include "stdafx.h"


// Object가 그려지기 위해서 필요한 정보들을 모아놓는다.

struct RenderItem
{
	RenderItem() = default;

	XMFLOAT4X4 World = MathHelper::Identity4x4();

	// 물체의 자료가 변해서 상수버퍼를 갱신해야하는지를 나타는 Dirty 플래그.
	// 프레임 리소스마다 물체의 상수버퍼가 있으므로 프레임 리소스마다 갱신을 적용해야한다. 
	// 따라서, 물체의 자료를 수정할때는 반드시 NumFramesDirty=gNumFrameResources를 해주자.
	// 이 플래그가 전체 프레임리소스 수와 같아야 Scene업데이트시 FrameIndex마다 돌면서 이 오브젝트의 상수버퍼를 전부 갱신할 것이다.
	// 최적화 필요없으면 상수버퍼 그냥 이런 플래그 없이 전부 갱신해도 되긴 하것다.
	int NumFramesDirty = gNumFrameResources;

	UINT ObjCBIndex = -1;		// 상수 버퍼 인덱스

	MeshGeometry* Geo = nullptr;

	// 위상구조
	D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	//DrawIndexedInstnaced 매개변수
	UINT m_IndexCount = 0;
	UINT StartIndexLocation = 0;
	UINT BaseVertexLocation = 0;
};
