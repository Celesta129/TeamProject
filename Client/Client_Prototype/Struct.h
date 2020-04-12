#pragma once
struct Vertex1
{
	XMFLOAT3 m_position;
	XMFLOAT3 m_normal;
	XMFLOAT2 m_texCoords;

	Vertex1() {}
	Vertex1(XMFLOAT3& pos, XMFLOAT3& normal, XMFLOAT2& tex) :m_position(pos),m_normal(normal),m_texCoords(tex)
	{}
};
