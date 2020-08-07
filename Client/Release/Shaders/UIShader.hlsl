
Texture2D    gDiffuseMap : register(t0);

SamplerState gsamPointWrap        : register(s0);
SamplerState gsamPointClamp       : register(s1);
SamplerState gsamLinearWrap       : register(s2);
SamplerState gsamLinearClamp      : register(s3);
SamplerState gsamAnisotropicWrap  : register(s4);
SamplerState gsamAnisotropicClamp : register(s5);

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
struct VertexIn
{
	float3 PosL : POSITION;
	float3 NormalL : NORMAL;
	float3 TangentL : TANGENT;
	float2 TexC : TEXCOORD;
	uint4 BoneIndices : BORNINDEX;
	float3 BoneWeights : WEIGHT;
	uint texindex : TEXINDEX;
};

struct VertexOut
{
	float4 PosH    : SV_POSITION;
	float3 PosW    : POSITION;
	float3 NormalW : NORMAL;
	float2 TexC    : TEXCOORD;
	float3 TangentW : TANGENT;
};


VertexOut VS(VertexIn vin)
{
	VertexOut vout = (VertexOut)0.0f;

	// Transform to homogeneous clip space. // ����������� ��ȯ�Ѵ�.
	float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
	vout.PosW = posW.xyz;

	// Assumes nonuniform scaling; otherwise, need to use inverse-transpose of world matrix.
	vout.NormalW = mul(vin.NormalL, (float3x3)gWorld);

	vout.TangentW = mul(vin.TangentL, (float3x3)gWorld);

	// Transform to homogeneous clip space.	 �������ܰ������� ��ȯ�Ѵ�.
	float4x4 InvView = gInvView;
	// ī�޶��� ����Ŀ� ��ġ�� �ٲ۴�.
	InvView._41 = posW.x; InvView._42 = posW.y; InvView._43 = posW.z;
	float4x4 InvViewProj = mul(InvView, gProj);
	vout.PosH = mul(posW, InvViewProj);	// 

	// Output vertex attributes for interpolation across triangle.
	float4 texC = mul(float4(vin.TexC, 0.0f, 1.0f), gTexTransform);
	vout.TexC = mul(texC, gMatTransform).xy;
	
	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	float4 diffuseAlbedo = gDiffuseMap.Sample(gsamAnisotropicWrap, pin.TexC) * gDiffuseAlbedo;
	

	// Interpolating normal can unnormalize it, so renormalize it.
	pin.NormalW = normalize(pin.NormalW);

	// Light terms.		// �����꿡 ���ԵǴ� �׵�
	float4 ambient =  diffuseAlbedo;

	float4 litColor = ambient;

	// Common convention to take alpha from diffuse albedo.
	litColor.a = diffuseAlbedo.a;

	return litColor;
}
