#include "Box.h"



CBox::CBox()
{
}


CBox::~CBox()
{
}

void CBox::Update(float fTimeElapsed)
{
	m_Pos.x += 1 * fTimeElapsed;
}

void CBox::Render(void)
{
	/*ID3D12DescriptorHeap* descriptorHeaps[] = { m_CbvHeap.Get() };
	m_GraphicsCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	m_GraphicsCommandList->SetGraphicsRootSignature(m_d3dGraphicsRootSignature.Get());

	m_GraphicsCommandList->IASetVertexBuffers(0, 1, &m_BoxGeo->VertexBufferView());
	m_GraphicsCommandList->IASetIndexBuffer(&m_BoxGeo->IndexBufferView());
	m_GraphicsCommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_GraphicsCommandList->SetGraphicsRootDescriptorTable(0, m_CbvHeap->GetGPUDescriptorHandleForHeapStart());

	m_GraphicsCommandList->DrawIndexedInstanced(
		m_BoxGeo->DrawArgs["box"].IndexCount,
		1, 0, 0, 0);*/
}
