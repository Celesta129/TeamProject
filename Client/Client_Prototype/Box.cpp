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
	MeshGeometry* pMeshGeo = m_RenderItem.Mesh->GetMeshGeo();

	pCommandList->IASetVertexBuffers(0, 1, &pMeshGeo->VertexBufferView());
	pCommandList->IASetIndexBuffer(&pMeshGeo->IndexBufferView());
	pCommandList->IASetPrimitiveTopology(m_RenderItem.PrimitiveType);

}
