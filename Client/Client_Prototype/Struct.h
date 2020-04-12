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

struct mesh
{
	vector<Vertex1> m_vertices;
	vector<int> m_indices;
	UINT m_materialIndex;
	mesh() {
		m_materialIndex = 0;
	}
};
