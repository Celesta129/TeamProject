#include "Weapon.h"
#include "Player.h"

void CWeapon::set_Player(CPlayer * pPlayer)
{
	m_pPlayer = pPlayer;
}

HRESULT CWeapon_Hammer::Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
	HRESULT hr = E_FAIL;
	hr = CModelObject::Initialize(L"Component_Model_Hammer", L"Texture_Hammer_diffuse", pd3dDevice, pd3dCommandList);
	if (FAILED(hr))
	{
		_MSG_BOX("CWeapon_Hammer init Failed");
		return hr;
	}
	return S_OK;
}

int CWeapon_Hammer::Update(float fTimeElapsed)
{
	if (m_pPlayer)
	{
		//hand를 찾자
		XMFLOAT4X4 hand = m_pPlayer->get_Hand();
		XMFLOAT4X4 playerWorld = m_pPlayer->Get_Transform()->Get_World();
		m_matParent = Matrix4x4::Multiply(hand, playerWorld);
		
		m_pTransform->Set_Parent(&m_matParent);

		XMFLOAT4X4 world = m_pTransform->Get_World();
		DirtyFrames();
	}
	else
	{
		m_pTransform->Set_Parent(nullptr);
	}

	return 0;
}

HRESULT CFlag::Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
	HRESULT hr = E_FAIL;
	hr = CModelObject::Initialize(L"Component_Model_Flag", L"Texture_Flag", pd3dDevice, pd3dCommandList);
	if (FAILED(hr))
	{
		_MSG_BOX("CFlag init Failed");
		return hr;
	}
	return S_OK;
}

int CFlag::Update(float fTimeElapsed)
{
	if (m_pPlayer)
	{
		////hand를 찾자
		//XMFLOAT4X4 hand = m_pPlayer->get_Hand();
		//XMFLOAT4X4 playerWorld = m_pPlayer->Get_Transform()->Get_World();
		//m_matParent = Matrix4x4::Multiply(hand, playerWorld);

		//m_pTransform->Set_Parent(&m_matParent);

		//XMFLOAT4X4 world = m_pTransform->Get_World();
		//DirtyFrames();
	}
	else
	{
		m_pTransform->Set_Parent(nullptr);
	}

	return 0;
}
