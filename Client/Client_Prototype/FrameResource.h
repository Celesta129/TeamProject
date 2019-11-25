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

	// 명령할당자는 GPU가 명령들을 다 처리한 후 재설정해야한므로 프레임마다 할당자가 필요함
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CmdListAlloc;

	// 상수버퍼는 참조하는 명령들을 GPU가 다 처리한 후에 갱신해야 함.
	// 프레임마다 상수버퍼를 새로 만들어야한다.

	std::unique_ptr<UploadBuffer<PassConstants>> PassCB = nullptr;
	std::unique_ptr<UploadBuffer<ObjectConstants>> ObjectCB = nullptr;

	// Fence는 현재 울타리 지점까지의 명령들을 표시하는 값
	// 이 값은 GPU가 아직 이 프레임 자원들을 사용하고있는지 판정하는 용도로 사용한다.
	UINT64 Fence = 0;

};

