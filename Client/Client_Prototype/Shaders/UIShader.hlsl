
Texture2D    gDiffuseMap : register(t0);

SamplerState gsamPointWrap        : register(s0);
SamplerState gsamPointClamp       : register(s1);
SamplerState gsamLinearWrap       : register(s2);
SamplerState gsamLinearClamp      : register(s3);
SamplerState gsamAnisotropicWrap  : register(s4);
SamplerState gsamAnisotropicClamp : register(s5);

#define NUM_MAX_UITEXTURE 4

Texture2D gUITextures1 : register(t9);
Texture2D gUITextures2 : register(t10);
Texture2D gUITextures3 : register(t11);
Texture2D gUITextures4 : register(t12);
Texture2D gUITextures5 : register(t13);

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

cbuffer cbPerObject : register(b1)
{
	float4x4 gWorld;
	float4x4 gTexTransform;
};
cbuffer cbMaterial  : register(b2)
{
	float4   gDiffuseAlbedo;
	float3   gFresnelR0;
	float    gRoughness;
	float4x4 gMatTransform;
}
cbuffer cbUI : register(b3)
{
	float2 f2ScreenPos;
	float2 f2ScreenSize;

	uint2 n2Size;
	//float2 f2Scale;
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


VertexOut VS(VertexIn vin)
{
	VertexOut output = (VertexOut)0.0f;

	float2 screenpos = (float2) 0.0f;
	screenpos.x = (gxmf2ScreenPos.x - gxmf2ScreenSize.x / 2.0f) / (gxmf2ScreenSize.x / 2.0f);
	screenpos.y = (gxmf2ScreenPos.y - gxmf2ScreenSize.y / 2.0f) / (gxmf2ScreenSize.y / 2.0f);

	float2 size = (float2) 0.0f;
	size.x = ((float)gnSize.x / gxmf2ScreenSize.x) * gfScale.x;
	size.y = ((float)gnSize.y / gxmf2ScreenSize.y) * gfScale.y;

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

	uv = float2(
	input.uv.x / gnNumSprite.x + float(gnNowSprite.x) / float(gnNumSprite.x),
	input.uv.y / gnNumSprite.y + float(gnNowSprite.y) / float(gnNumSprite.y)
	);

	finalColor = gDiffuseMap.Sample(gDefaultSamplerState, uv);
	finalColor.a *= gfAlpha;

	return finalColor;
}
