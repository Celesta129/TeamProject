#pragma once
struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};
struct ObjectConstants {
	XMFLOAT4X4 WorldViewProj = MathHelper::Identity4x4();
};