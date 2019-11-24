#include "Box.h"
#include "Mesh.h"


CBox::CBox()
{
	
}


CBox::~CBox()
{
}

void CBox::Update(float fTimeElapsed)
{
	//m_Pos.x += 1 * fTimeElapsed;
}

void CBox::Render(ID3D12GraphicsCommandList* pCommandList)
{
	MeshGeometry* pMeshGeo = m_pMesh->GetMeshGeo();

	ID3D12DescriptorHeap* descriptorHeaps[] = { m_CbvHeap.Get() };
	pCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	pCommandList->SetGraphicsRootSignature(m_d3dGraphicsRootSignature.Get());

	pCommandList->IASetVertexBuffers(0, 1, &pMeshGeo->VertexBufferView());
	pCommandList->IASetIndexBuffer(&pMeshGeo->IndexBufferView());
	pCommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	pCommandList->SetGraphicsRootDescriptorTable(0, m_CbvHeap->GetGPUDescriptorHandleForHeapStart());

	pCommandList->DrawIndexedInstanced(
		pMeshGeo->DrawArgs["box"].IndexCount,
		1, 0, 0, 0);
}
