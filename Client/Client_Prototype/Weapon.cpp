#include "Weapon.h"
#include "Player.h"

void CWeapon::set_Player(CPlayer * pPlayer)
{
	m_pPlayer = pPlayer;
}

HRESULT CWeapon::Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
	HRESULT hr = E_FAIL;
	wstring modeltag = L"Component_Model_Hammer";
	wstring texturetag = L"Texture_Hammer_diffuse";
	switch (m_type)
	{
		
	case WEAPON_HAMMER:
		modeltag = L"Component_Model_Hammer";
		texturetag = L"Texture_Hammer_diffuse";
		break;
	case WEAPON_BLOCK:
		modeltag = L"Component_Model_Block";
		texturetag = L"Texture_Block_diffuse";
		break;
	case WEAPON_SNACK:
		modeltag = L"Component_Model_Snack";
		texturetag = L"Texture_Snack_diffuse";
		break;
	case WEAPON_SWORD:
		modeltag = L"Component_Model_Sword";
		texturetag = L"Texture_Sword_diffuse";
		break;
	case FLAG:
		modeltag = L"Component_Model_Flag";
		texturetag = L"Texture_Flag";
		break;
	default:
		break;
	}
	hr = CModelObject::Initialize(modeltag, texturetag, pd3dDevice, pd3dCommandList);
	if (FAILED(hr))
	{
		_MSG_BOX("CWeapon init Failed");
		return hr;
	}
	return S_OK;
}

int CWeapon::Update(float fTimeElapsed)
{
	switch (m_type)
	{
	default:
		break;
	}
	if (m_pPlayer)
	{
		//hand¸¦ Ã£ÀÚ
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
