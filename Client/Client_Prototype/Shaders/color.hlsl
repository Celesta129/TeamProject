
cbuffer cbCamera : register(b0)
{
	float4x4 gView;
	float4x4 gProj;
}
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
	float4x4 viewproj = mul(gView, gProj);
	vout.PosH = mul(posW, viewproj);

	// Just pass vertex color into the pixel shader.
	vout.Color = vin.Color;

	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	return pin.Color;
}