
Texture2D    gDiffuseMap : register(t0);

SamplerState gsamPointWrap        : register(s0);
SamplerState gsamPointClamp       : register(s1);
SamplerState gsamLinearWrap       : register(s2);
SamplerState gsamLinearClamp      : register(s3);
SamplerState gsamAnisotropicWrap  : register(s4);
SamplerState gsamAnisotropicClamp : register(s5);

#define NUM_MAX_UITEXTURE 4

cbuffer cbPass : register(b0)
{
	float4x4 gView;
	float4x4 gInvView;
	float4x4 gProj;
	float4x4 gInvProj;
	float4x4 gViewProj;
	float4x4 gInvViewProj;
	float3 gEyePosW;
	float cbPerObjectPad1;
	float2 gRenderTargetSize;
	float2 gInvRenderTargetSize;
	float gNearZ;
	float gFarZ;
	float gTotalTime;
	float gDeltaTime;
	float4 gAmbientLight;

};

cbuffer cbUI : register(b1)
{
	float2 f2ScreenPos;			// UI의 화면상 좌표
	//float2 f2ScreenSize;		// 화면해상도?

	float2 f2Size;	// 기본 UI 크기
	float2 f2Scale;	// UI크기가 유동적으로 변해야할때.

	float alpha;
}
struct VertexIn
{
	float3 position : POSITION; // 
	float2 uv : TEXCOORD; // 
};

struct VertexOut
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};


VertexOut VS(uint nVertexID : SV_VertexID)
{
	VertexOut output = (VertexOut)0.0f;

	float2 screenpos = (float2) 0.0f;
	screenpos.x = (f2ScreenPos.x - gRenderTargetSize.x / 2.0f) / (gRenderTargetSize.x / 2.0f);
	screenpos.y = (f2ScreenPos.y - gRenderTargetSize.y / 2.0f) / (gRenderTargetSize.y / 2.0f);

	float2 size = (float2) 0.0f;
	size.x = ((float)f2Size.x / gRenderTargetSize.x) * f2Scale.x;
	size.y = ((float)f2Size.y / gRenderTargetSize.y) * f2Scale.y;

	if (nVertexID == 0)
	{
		output.uv = (float2(0.0f, 0.0f)); // 스크린 왼쪽 위 
		output.position = float4(screenpos.x - size.x, screenpos.y + size.y, 0.0f, 1.0f);
	}
	if (nVertexID == 1)
	{
		output.uv = (float2(1.0f, 0.0f)); // 스크린 오른쪽 위
		output.position = float4(screenpos.x + size.x, screenpos.y + size.y, 0.0f, 1.0f);

	}
	if (nVertexID == 2)
	{
		output.uv = (float2(1.0f, 1.0f)); // 스크린 오른쪽 아래
		output.position = float4(screenpos.x + size.x, screenpos.y - size.y, 0.0f, 1.0f);

	}
	if (nVertexID == 3)
	{
		output.uv = (float2(0.0f, 0.0f)); // 스크린 왼쪽 위
		output.position = float4(screenpos.x - size.x, screenpos.y + size.y, 0.0f, 1.0f);

	}
	if (nVertexID == 4)
	{
		output.uv = (float2(1.0f, 1.0f)); // 스크린 왼쪽 아래
		output.position = float4(screenpos.x + size.x, screenpos.y - size.y, 0.0f, 1.0f);

	}
	if (nVertexID == 5)
	{
		output.uv = (float2(0.0f, 1.0f)); // 스크린 오른쪽 아래
		output.position = float4(screenpos.x - size.x, screenpos.y - size.y, 0.0f, 1.0f);
	}

	return output;
}

float4 PS(VertexOut pin) : SV_Target
{
	float4 finalColor = (float4) 0.0f;
	float2 uv = pin.uv;

	//uv = float2(
	//input.uv.x / gnNumSprite.x + float(gnNowSprite.x) / float(gnNumSprite.x),
	//input.uv.y / gnNumSprite.y + float(gnNowSprite.y) / float(gnNumSprite.y)
	//);

	finalColor = gDiffuseMap.Sample(gsamAnisotropicWrap, uv);
	finalColor.a *= alpha;

	return finalColor;
}
