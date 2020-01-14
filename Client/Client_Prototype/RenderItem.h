#pragma once
#include "stdafx.h"


// Object�� �׷����� ���ؼ� �ʿ��� �������� ��Ƴ��´�.

struct RenderItem
{
	RenderItem() = default;

	XMFLOAT4X4 World = MathHelper::Identity4x4();

	// ��ü�� �ڷᰡ ���ؼ� ������۸� �����ؾ��ϴ����� ��Ÿ�� Dirty �÷���.
	// ������ ���ҽ����� ��ü�� ������۰� �����Ƿ� ������ ���ҽ����� ������ �����ؾ��Ѵ�. 
	// ����, ��ü�� �ڷḦ �����Ҷ��� �ݵ�� NumFramesDirty=gNumFrameResources�� ������.
	// �� �÷��װ� ��ü �����Ӹ��ҽ� ���� ���ƾ� Scene������Ʈ�� FrameIndex���� ���鼭 �� ������Ʈ�� ������۸� ���� ������ ���̴�.
	// ����ȭ �ʿ������ ������� �׳� �̷� �÷��� ���� ���� �����ص� �Ǳ� �ϰʹ�.
	int NumFramesDirty = gNumFrameResources;

	UINT ObjCBIndex = -1;		// ��� ���� �ε���

	MeshGeometry* Geo = nullptr;

	// ������
	D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	//DrawIndexedInstnaced �Ű�����
	UINT m_IndexCount = 0;
	UINT StartIndexLocation = 0;
	UINT BaseVertexLocation = 0;
};
