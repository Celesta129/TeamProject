#pragma once
struct VertexData
{
	XMFLOAT3   m_pos;
	XMFLOAT3   m_normal;
	XMFLOAT3   m_tan;
	XMFLOAT2   m_tex;
	XMUINT4    m_bornIndex;
	XMFLOAT3   m_weights;
	UINT      m_nTextureNum = 0;

	VertexData() {}
	VertexData(XMFLOAT3& pos, XMFLOAT3& normal, XMFLOAT3& tan, XMFLOAT2& tex, UINT texindex) : m_pos(pos), m_normal(normal), m_tan(tan), m_tex(tex), m_nTextureNum(texindex)
	{
		m_weights = XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_bornIndex = XMUINT4(0, 0, 0, 0);
	}
	void AddBoneData(UINT index, float weight) {
		if (m_weights.x == 0.0f) {
			m_bornIndex.x = index;
			m_weights.x = weight;
		}
		else if (m_weights.y == 0.0f) {
			m_bornIndex.y = index;
			m_weights.y = weight;
		}
		else if (m_weights.z == 0.0f) {
			m_bornIndex.z = index;
			m_weights.z = weight;
		}
		else {
			m_bornIndex.w = index;
		}
	}
};
struct Bone
{
	XMMATRIX   BoneOffset;
	XMMATRIX FinalTransformation;

	Bone() {
		BoneOffset = XMMatrixIdentity();
		FinalTransformation = XMMatrixIdentity();
	}
};

struct mesh
{
	vector<VertexData>      m_vertices;
	vector<int>            m_indices;
	UINT               m_materialIndex;

	mesh() { m_materialIndex = 0; }
	~mesh() {
		//cout << "mesh ¼Ò¸êÀÚ" << endl;
		m_vertices.clear();
		m_indices.clear();
	}
	void SetMeshesTextureIndex(UINT index) {
		for (auto& d : m_vertices)
			d.m_nTextureNum = index;
	}

};