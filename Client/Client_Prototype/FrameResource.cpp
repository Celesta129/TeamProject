#include "FrameResource.h"


FrameResource::FrameResource(ID3D12Device * device, UINT passCount, UINT objectCount)
{
	ThrowIfFailed(device->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(CmdListAlloc.GetAddressOf())));
	if(passCount > 0)
		PassCB = std::make_unique<UploadBuffer<PassConstants>>(device, passCount, true);
	if (objectCount > 0)
	{
		ObjectCB = std::make_unique<UploadBuffer<ObjectConstants>>(device, objectCount, true);
		MaterialCB = std::make_unique<UploadBuffer<MaterialConstants>>(device, objectCount, true);
		SkinnedCB = std::make_unique<UploadBuffer<SkinnedConstants>>(device, objectCount, true);
		UICB = std::make_unique<UploadBuffer<UI_Constants>>(device, objectCount, true);
	}
}

FrameResource::~FrameResource()
{
}
