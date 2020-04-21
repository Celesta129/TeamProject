#include "Mesh.h"
#include "FrameResource.h"


CMesh::CMesh()
{
}

CMesh::~CMesh()
{
	ReleaseUploadBuffers();
}


void CMesh::BuildMeshGeo(ID3D12Device* pDevice, ID3D12GraphicsCommandList * pCommandList)
{
	
}

void CMesh::CopyMesh(shared_ptr<CMesh> pMesh)
{
	m_pd3dVertexBuffer = pMesh->m_pd3dVertexBuffer;
	m_pd3dVertexUploadBuffer = pMesh->m_pd3dVertexUploadBuffer;
	
	m_pd3dIndexBuffer = pMesh->m_pd3dIndexBuffer;
	m_pd3dIndexUploadBuffer = pMesh->m_pd3dIndexUploadBuffer;
	
	m_d3dVertexBufferView = pMesh->m_d3dVertexBufferView;
	m_d3dIndexBufferView = pMesh->m_d3dIndexBufferView;

	m_d3dPrimitiveTopology = pMesh->m_d3dPrimitiveTopology;

	m_nSlot = pMesh->m_nSlot;
	m_nVertices = pMesh->m_nVertices;
	m_nStride = pMesh->m_nStride;
	m_nOffset = pMesh->m_nOffset;
	m_nIndices = pMesh->m_nIndices;

	m_nStartIndex = pMesh->m_nStartIndex;
	m_nBaseVertex = pMesh->m_nBaseVertex;
}

void CMesh::ReleaseUploadBuffers(void)
{
	if (m_pd3dVertexBuffer) {
		m_pd3dVertexBuffer->Release();
		m_pd3dVertexBuffer = nullptr;
	}
	if (m_pd3dVertexUploadBuffer) {
		m_pd3dVertexUploadBuffer->Release();
		m_pd3dVertexUploadBuffer = nullptr;
	}
	if (m_pd3dIndexBuffer) {
		m_pd3dIndexBuffer->Release();
		m_pd3dIndexBuffer = nullptr;
	} 
	if (m_pd3dIndexUploadBuffer) {
		m_pd3dIndexUploadBuffer->Release();
		m_pd3dIndexUploadBuffer = nullptr;
	} 
}

void CMesh::Render(ID3D12GraphicsCommandList * pd3dCommandList, UINT nInstanceCount)
{
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);		
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dVertexBufferView);	
	if (m_pd3dIndexBuffer)
	{
		pd3dCommandList->IASetIndexBuffer(&m_d3dIndexBufferView);
		pd3dCommandList->DrawIndexedInstanced(m_nIndices, nInstanceCount, 0, 0, 0);		// 몇번째 인스턴스 사용할지 결정
	}
	else
	{
		pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
	}
}


CBoxMesh::CBoxMesh(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList)
	
{
	BuildMeshGeo(pDevice, pCommandList);
}

CBoxMesh::~CBoxMesh()
{
}

void CBoxMesh::BuildMeshGeo(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList)
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

	m_MeshGeo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(pDevice,
		pCommandList, vertices.data(), vbByteSize, m_MeshGeo->VertexBufferUploader);

	m_MeshGeo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(pDevice,
		pCommandList, indices.data(), ibByteSize, m_MeshGeo->IndexBufferUploader);

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

MMesh::MMesh(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
}

MMesh::~MMesh()
{
}

void MMesh::ReleaseUploadBuffers()
{
}

void MMesh::Render(ID3D12GraphicsCommandList * pd3dCommandList)
{
}

void MMesh::Render(ID3D12GraphicsCommandList * pd3dCommandList, UINT nInstanceCount)
{
}
