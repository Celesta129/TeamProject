#include "Mesh.h"




CMesh::CMesh(ID3D12Device * pDevice, ID3D12GraphicsCommandList * pCommandList)
	:m_d3dDevice(pDevice), m_GraphicsCommandList(pCommandList)
{
}


CMesh::~CMesh()
{
}

void CMesh::BuildMeshGeo(void)
{
	
}

CBoxMesh::CBoxMesh(ID3D12Device * pDevice, ID3D12GraphicsCommandList * pCommandList)
	:CMesh(pDevice, pCommandList)
{
	BuildMeshGeo();
}

CBoxMesh::~CBoxMesh()
{
}

void CBoxMesh::BuildMeshGeo(void)
{
	std::array<Vertex, 8> vertices =
	{
		Vertex({ XMFLOAT3(-1.f, -1.f, -1.f), XMFLOAT4(Colors::White) }),
		Vertex({ XMFLOAT3(-1.f, +1.f, -1.f), XMFLOAT4(Colors::Black) }),
		Vertex({ XMFLOAT3(+1.f, +1.f, -1.f), XMFLOAT4(Colors::Red) }),
		Vertex({ XMFLOAT3(+1.f, -1.f, -1.f), XMFLOAT4(Colors::Green) }),
		Vertex({ XMFLOAT3(-1.f, -1.f, +1.f), XMFLOAT4(Colors::Blue) }),
		Vertex({ XMFLOAT3(-1.f, +1.f, +1.f), XMFLOAT4(Colors::Yellow) }),
		Vertex({ XMFLOAT3(+1.f, +1.f, +1.f), XMFLOAT4(Colors::Cyan) }),
		Vertex({ XMFLOAT3(+1.f, -1.f, +1.f), XMFLOAT4(Colors::Magenta) })
	};

	std::array<std::uint16_t, 36> indices =
	{
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3,
		4, 3, 7
	};

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

	m_MeshGeo = std::make_unique<MeshGeometry>();
	m_MeshGeo->Name = "boxGeo";

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &m_MeshGeo->VertexBufferCPU));
	CopyMemory(m_MeshGeo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize, &m_MeshGeo->IndexBufferCPU));
	CopyMemory(m_MeshGeo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	m_MeshGeo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(m_d3dDevice.Get(),
		m_GraphicsCommandList.Get(), vertices.data(), vbByteSize, m_MeshGeo->VertexBufferUploader);

	m_MeshGeo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(m_d3dDevice.Get(),
		m_GraphicsCommandList.Get(), indices.data(), ibByteSize, m_MeshGeo->IndexBufferUploader);

	m_MeshGeo->VertexByteStride = sizeof(Vertex);
	m_MeshGeo->VertexBufferByteSize = vbByteSize;
	m_MeshGeo->IndexFormat = DXGI_FORMAT_R16_UINT;
	m_MeshGeo->IndexBufferByteSize = ibByteSize;

	SubmeshGeometry submesh;
	submesh.IndexCount = (UINT)indices.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	m_MeshGeo->DrawArgs["box"] = submesh;
}
