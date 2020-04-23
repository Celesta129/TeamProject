#include "Mesh.h"

CMesh::CMesh()
{
}

CMesh::~CMesh()
{
	ReleaseUploadBuffers();
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
