#include "FrameResource.h"


FrameResource::FrameResource(ID3D12Device * device, UINT passCount, UINT objectCount, UINT materialCount)
{
	ThrowIfFailed(device->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(CmdListAlloc.GetAddressOf())));
	if(passCount > 0)
		PassCB = std::make_unique<UploadBuffer<PassConstants>>(device, passCount, true);
	if(objectCount > 0)
		ObjectCB = std::make_unique<UploadBuffer<ObjectConstants>>(device, objectCount, true);
	if(materialCount > 0)
		MaterialCB = std::make_unique<UploadBuffer<MaterialConstants>>(device, materialCount, true);
}

FrameResource::~FrameResource()
{
}
