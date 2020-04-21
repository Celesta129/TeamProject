
#include "GameObject.h"
#include "Component_Manager.h"

CGameObject::CGameObject()
{
	
}


CGameObject::~CGameObject()
{
	
}

HRESULT CGameObject::Initialize(void)
{
	// 컴포넌트매니저의 포인터를 획득
	m_pComponent_Manager = CComponent_Manager::GetInstance();
	// Object는 기본적으로 Transform을 멤버로 갖게 있게 설계했다. 취향껏 바꾸자.
	
	if (FAILED(Insert_Component_ToMap(L"Component_Transform")))
		return E_FAIL;
	
	return S_OK;
}

void CGameObject::Release(void)
{
	for (auto& e : m_mapComponent)
	{
		e.second->Release();
	}
	m_mapComponent.clear();
}

void CGameObject::Update(float fTimeElapsed)
{

}

void CGameObject::Render(ID3D12GraphicsCommandList* pCommandList)
{
	//pd3dCommandList->SetGraphicsRootDescriptorTable(1, m_d3dCbvGPUDescriptorHandle);

	////if (m_pShader) m_pShader->Render(pd3dCommandList, pCamera);
	//if (m_pMesh) m_pMesh->Render(pd3dCommandList);

	// m_pMesh -> render define
	/*pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dVertexBufferView);

	if (m_pd3dIndexBuffer)
	{
		pd3dCommandList->IASetIndexBuffer(&m_d3dIndexBufferView);
		pd3dCommandList->DrawIndexedInstanced(m_nIndices, nInstances, 0, 0, 0);
	}
	else
	{
		pd3dCommandList->DrawInstanced(m_nVertices, nInstances, m_nOffset, 0);
	}*/
}

ObjectConstants CGameObject::GetObjectConstants(void)
{
	ObjectConstants result = ObjectConstants();
	CTransform* pTransform = (CTransform*)Get_Component(L"Component_Transform");
	if (pTransform == nullptr)
		return result;

	XMMATRIX world = XMLoadFloat4x4(&pTransform->Get_World());
	XMStoreFloat4x4(&result.World, XMMatrixTranspose(world));

	return result;
}

CComponent * CGameObject::Get_Component(const wstring tag)
{
	auto iter = find_if(m_mapComponent.begin(), m_mapComponent.end(), Finder_Tag(tag));
	if (iter == m_mapComponent.end())
		return nullptr;
	return iter->second;
}

HRESULT  CGameObject::Insert_Component_ToMap(const wstring tag)
{
	CComponent* pComponent = nullptr;
	pComponent = m_pComponent_Manager->Clone_Component(tag);
	if (pComponent == nullptr)
	{
		_MSG_BOX("Target Component가 매니저에 없거나 Clone을 거지같이함")
			return E_FAIL;
	}
	m_mapComponent.insert(make_pair(tag, pComponent));

	return S_OK;
}

