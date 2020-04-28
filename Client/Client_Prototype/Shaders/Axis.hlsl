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
};

cbuffer cbPerObject : register(b1)
{
	float4x4 gWorld;
};

struct VertexIn
{
	float3 PosL  : POSITION;
	float4 Color : COLOR;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
	float4 Color : COLOR;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	// Transform to homogeneous clip space.
	float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
	vout.PosH = mul(posW, gViewProj);


	vout.Color = vin.Color;
	if (gWorld[3][0] > 0.0f) {
		vout.Color.x = 255.f;
	}
	if (gWorld[3][1] > 0.0f) {
		vout.Color.y = 255.f;
	}
	if (gWorld[3][2] > 0.0f) {
		vout.Color.z = 255.f;
	}
	// Just pass vertex color into the pixel shader.
	
	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	return pin.Color;
}
