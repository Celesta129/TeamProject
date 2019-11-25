#pragma once
#include "stdafx.h"
#include "../Common/UploadBuffer.h"
struct FrameResource
{
public:
	FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount);
	FrameResource(const FrameResource& rhs) = delete;
	FrameResource& operator=(const FrameResource& rhs) = delete;
	~FrameResource();

	// ����Ҵ��ڴ� GPU�� ��ɵ��� �� ó���� �� �缳���ؾ��ѹǷ� �����Ӹ��� �Ҵ��ڰ� �ʿ���
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CmdListAlloc;

	// ������۴� �����ϴ� ��ɵ��� GPU�� �� ó���� �Ŀ� �����ؾ� ��.
	// �����Ӹ��� ������۸� ���� �������Ѵ�.

	std::unique_ptr<UploadBuffer<PassConstants>> PassCB = nullptr;
	std::unique_ptr<UploadBuffer<ObjectConstants>> ObjectCB = nullptr;

	// Fence�� ���� ��Ÿ�� ���������� ��ɵ��� ǥ���ϴ� ��
	// �� ���� GPU�� ���� �� ������ �ڿ����� ����ϰ��ִ��� �����ϴ� �뵵�� ����Ѵ�.
	UINT64 Fence = 0;

};

