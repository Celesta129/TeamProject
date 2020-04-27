#pragma once
#include "stdafx.h"
#include "../Common/UploadBuffer.h"
#include "../Common/d3dUtil.h"
#include "../Common/MathHelper.h"
#include "../Common/UploadBuffer.h"

struct ObjectConstants
{
	DirectX::XMFLOAT4X4 World = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 TexTransform = MathHelper::Identity4x4();
};
struct SkinnedConstants
{
	DirectX::XMFLOAT4X4 BoneTransforms[96];
};
struct PassConstants
{
	DirectX::XMFLOAT4X4 View = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 InvView = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 Proj = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 InvProj = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 ViewProj = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 InvViewProj = MathHelper::Identity4x4();
	DirectX::XMFLOAT3 EyePosW = { 0.0f, 0.0f, 0.0f };
	float cbPerObjectPad1 = 0.0f;
	DirectX::XMFLOAT2 RenderTargetSize = { 0.0f, 0.0f };
	DirectX::XMFLOAT2 InvRenderTargetSize = { 0.0f, 0.0f };
	float NearZ = 0.0f;
	float FarZ = 0.0f;
	//float TotalTime = 0.0f;
	//float DeltaTime = 0.0f;
	DirectX::XMFLOAT4 AmbientLight = { 0.0f, 0.0f, 0.0f, 1.0f };
	// Indices [0, NUM_DIR_LIGHTS) are directional lights;
   // indices [NUM_DIR_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHTS) are point lights;
   // indices [NUM_DIR_LIGHTS+NUM_POINT_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHT+NUM_SPOT_LIGHTS)
   // are spot lights for a maximum of MaxLights per object.
	Light Lights[MaxLights];
};

struct Vertex
{
	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT2 TexC;
};

struct FrameResource
{
public:
	FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount);
	FrameResource(const FrameResource& rhs) = delete;
	FrameResource& operator=(const FrameResource& rhs) = delete;
	~FrameResource();

	// ����Ҵ��ڴ� GPU�� ��ɵ��� �� ó���� �� �缳���ؾ��ϹǷ� �����Ӹ��� �Ҵ��ڰ� �ʿ���
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CmdListAlloc;

	// ������۴� �����ϴ� ��ɵ��� GPU�� �� ó���� �Ŀ� �����ؾ� ��.
	// �����Ӹ��� ������۸� ���� �������Ѵ�.

	std::unique_ptr<UploadBuffer<PassConstants>> PassCB = nullptr;
	std::unique_ptr<UploadBuffer<MaterialConstants>> MaterialCB = nullptr;
	std::unique_ptr<UploadBuffer<SkinnedConstants>> SkinnedCB = nullptr;
	std::unique_ptr<UploadBuffer<ObjectConstants>> ObjectCB = nullptr;

	// Fence�� ���� ��Ÿ�� ���������� ��ɵ��� ǥ���ϴ� ��
	// �� ���� GPU�� ���� �� ������ �ڿ����� ����ϰ��ִ��� �����ϴ� �뵵�� ����Ѵ�.
	UINT64 Fence = 0;

};

