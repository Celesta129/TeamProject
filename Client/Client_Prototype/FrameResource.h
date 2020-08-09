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
	SkinnedConstants() {
		for (int i = 0; i < 96; ++i) {
			BoneTransforms[i] = MathHelper::Identity4x4();
		}
	}
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
struct UI_Constants
{
	XMFLOAT2 xmf2ScreenPos = XMFLOAT2(0.f,0.f);		// 화면상 좌표
	//XMFLOAT2 xmf2ScreenSize = XMFLOAT2(0.f,0.f);	// 화면 사이즈

	XMFLOAT2 xmf2UISize = XMFLOAT2(0.f,0.f);			// UI 사이즈
	XMFLOAT2 xmf2Scale = XMFLOAT2(1.f, 1.f);		//

	XMINT2 nowSprite = XMINT2(0, 0);
	XMINT2 numSprite = XMINT2(0, 0);

	float alpha = 1.f;

	float data = 0.f;
};
struct FrameResource
{
public:
	FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount);
	FrameResource(const FrameResource& rhs) = delete;
	FrameResource& operator=(const FrameResource& rhs) = delete;
	~FrameResource();

	// 명령할당자는 GPU가 명령들을 다 처리한 후 재설정해야하므로 프레임마다 할당자가 필요함
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CmdListAlloc;

	// 상수버퍼는 참조하는 명령들을 GPU가 다 처리한 후에 갱신해야 함.
	// 프레임마다 상수버퍼를 새로 만들어야한다.

	std::unique_ptr<UploadBuffer<PassConstants>> PassCB = nullptr;
	std::unique_ptr<UploadBuffer<MaterialConstants>> MaterialCB = nullptr;
	std::unique_ptr<UploadBuffer<SkinnedConstants>> SkinnedCB = nullptr;
	std::unique_ptr<UploadBuffer<ObjectConstants>> ObjectCB = nullptr;
	std::unique_ptr<UploadBuffer<UI_Constants>> UICB = nullptr;

	// Fence는 현재 울타리 지점까지의 명령들을 표시하는 값
	// 이 값은 GPU가 아직 이 프레임 자원들을 사용하고있는지 판정하는 용도로 사용한다.
	UINT64 Fence = 0;

};

