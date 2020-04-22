#pragma once
#include "stdafx.h"
#include "Component.h"
#include "Vertex.h"


inline XMMATRIX aiMatrixToXMMatrix(const aiMatrix4x4& offset)
{
	return XMMATRIX(&offset.a1);
}


class CMesh 
{
public:
	CMesh();
	virtual ~CMesh();
protected:
	ComPtr<ID3D12Resource>			m_pd3dVertexBuffer = NULL;
	ComPtr<ID3D12Resource>			m_pd3dVertexUploadBuffer = NULL;

	ComPtr<ID3D12Resource>			m_pd3dIndexBuffer = NULL;
	ComPtr<ID3D12Resource>			m_pd3dIndexUploadBuffer = NULL;

	D3D12_VERTEX_BUFFER_VIEW		m_d3dVertexBufferView;
	D3D12_INDEX_BUFFER_VIEW			m_d3dIndexBufferView;

	D3D12_PRIMITIVE_TOPOLOGY		m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	UINT							m_nSlot = 0;
	UINT							m_nVertices = 0;
	UINT							m_nStride = 0;
	UINT							m_nOffset = 0;

	UINT							m_nIndices = 0;
	UINT							m_nStartIndex = 0;
	int								m_nBaseVertex = 0;
public:
	void CopyMesh(shared_ptr<CMesh> pMesh);
	virtual void ReleaseUploadBuffers(void);

	BoundingOrientedBox				m_xmOOBB;
	//virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, UINT nInstanceCount = 1);

protected:
	unique_ptr<MeshGeometry> m_MeshGeo;
public:
	virtual void BuildMeshGeo(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList);

	MeshGeometry* GetMeshGeo(void) {
		return m_MeshGeo.get();
	};

};

class CBoxMesh : public CMesh 
{
public:
	CBoxMesh(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CBoxMesh();

	virtual void BuildMeshGeo(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList);
};

class MMesh
{
public:
	MMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual ~MMesh();

private:
	int								m_nReferences = 0;

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	void ReleaseUploadBuffers();

protected:
	ComPtr<ID3D12Resource>			m_pd3dVertexBuffer = NULL;
	ComPtr<ID3D12Resource>			m_pd3dVertexUploadBuffer = NULL;

	ComPtr<ID3D12Resource>			m_pd3dIndexBuffer = NULL;
	ComPtr<ID3D12Resource>			m_pd3dIndexUploadBuffer = NULL;

	D3D12_VERTEX_BUFFER_VIEW		m_d3dVertexBufferView;
	D3D12_INDEX_BUFFER_VIEW			m_d3dIndexBufferView;

	D3D12_PRIMITIVE_TOPOLOGY		m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	UINT							m_nSlot = 0;
	UINT							m_nVertices = 0;
	UINT							m_nStride = 0;
	UINT							m_nOffset = 0;

	UINT							m_nIndices = 0;
	UINT							m_nStartIndex = 0;
	int								m_nBaseVertex = 0;
public:
	BoundingOrientedBox				m_xmOOBB;
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, UINT nInstanceCount);
};


inline void CalculateTangentArray(UINT vertexCount, vector<VertexData>& vertices, long triangleCount, vector<int>& indeies)
{
	XMFLOAT3 *tan1 = new XMFLOAT3[vertexCount * 2];
	XMFLOAT3 *tan2 = tan1 + vertexCount;
	::ZeroMemory(tan1, vertexCount * sizeof(XMFLOAT3) * 2);

	for (long a = 0; a < triangleCount; a++)
	{
		UINT i1 = indeies[a * 3 + 0];
		UINT i2 = indeies[a * 3 + 1];
		UINT i3 = indeies[a * 3 + 2];

		const XMFLOAT3& v1 = vertices[i1].m_pos;
		const XMFLOAT3& v2 = vertices[i2].m_pos;
		const XMFLOAT3& v3 = vertices[i3].m_pos;

		const XMFLOAT2& w1 = vertices[i1].m_tex;
		const XMFLOAT2& w2 = vertices[i2].m_tex;
		const XMFLOAT2& w3 = vertices[i3].m_tex;

		float x1 = v2.x - v1.x;
		float x2 = v3.x - v1.x;
		float y1 = v2.y - v1.y;
		float y2 = v3.y - v1.y;
		float z1 = v2.z - v1.z;
		float z2 = v3.z - v1.z;

		float s1 = w2.x - w1.x;
		float s2 = w3.x - w1.x;
		float t1 = w2.y - w1.y;
		float t2 = w3.y - w1.y;

		float r = 1.0F / (s1 * t2 - s2 * t1);
		XMFLOAT3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
			(t2 * z1 - t1 * z2) * r);
		XMFLOAT3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
			(s1 * z2 - s2 * z1) * r);

		tan1[i1] = Vector3::Add(tan1[i1], sdir);
		tan1[i2] = Vector3::Add(tan1[i2], sdir);
		tan1[i3] = Vector3::Add(tan1[i3], sdir);

		tan2[i1] = Vector3::Add(tan2[i1], tdir);
		tan2[i2] = Vector3::Add(tan2[i2], tdir);
		tan2[i3] = Vector3::Add(tan2[i3], tdir);

	}

	for (UINT a = 0; a < vertexCount; a++)
	{
		XMFLOAT3& n = vertices[a].m_normal;
		XMFLOAT3& t = tan1[a];

		// Gram-Schmidt orthogonalize
		XMFLOAT3 SP_1 = Vector3::ScalarProduct(n, Vector3::DotProduct(n, t), false);
		XMFLOAT3 SP_2 = Vector3::Subtract(t, SP_1, false);
		vertices[a].m_tan = Vector3::Normalize(SP_2);

		// Calculate handedness
		//tangent[a].w = (Dot(Cross(n, t), tan2[a]) < 0.0F) ? -1.0F : 1.0F;
	}

	delete[] tan1;
}