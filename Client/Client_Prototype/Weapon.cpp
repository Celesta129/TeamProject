#include "Weapon.h"
#include "Player.h"

void CWeapon::set_Player(CPlayer * pPlayer)
{
	m_pPlayer = pPlayer;
	set_Type(m_type);

	XMFLOAT3 pos = XMFLOAT3(0.f, 0.f, 0.f);
	XMFLOAT3 rotate = XMFLOAT3(0.f, 0.f, 0.f);
	XMFLOAT3 scale = XMFLOAT3(2.f, 2.f, 2.f);
	if (m_pPlayer != nullptr)
	{
		m_pTransform->Set_Pos(pos);
		m_pTransform->Set_Rotate(rotate.x, rotate.y, rotate.z);
		m_pTransform->Set_Scale(scale);
		switch (m_type)
		{
		case WEAPON_SWORD:
			pos = XMFLOAT3(32.f, -33.f, -11.f);
			rotate = XMFLOAT3(90.f, 0.f, 0.f);
			scale = XMFLOAT3(0.5f, 0.5f, 0.5f);
			break;
		case WEAPON_HAMMER:
			rotate = XMFLOAT3(0.f, -90.f, 90.f);
			scale = XMFLOAT3(1.f, 1.f, 1.f);
			break;
		case WEAPON_BLOCK:

			break;
		case WEAPON_SNACK:
			scale = XMFLOAT3(0.5f, 0.5f, 0.5f);
			break;
		case FLAG:
			pos = XMFLOAT3(5.f, -33.f, -14.f);
			rotate = XMFLOAT3(0.f, -90.f, 0.f);
			scale = XMFLOAT3(1.f, 1.f, 1.f);
			break;
		}

		m_pTransform->Set_Pos(pos);
		m_pTransform->Set_Rotate(rotate.x, rotate.y, rotate.z);
		m_pTransform->Set_Scale(scale);
	}
}

void CWeapon::set_Type(UINT weapontype)
{
	m_type = weapontype;
	wstring modeltag = L"Component_Model_Sword";
	wstring texturetag = L"Texture_Sword_diffuse";

	
	XMFLOAT3 rotate = XMFLOAT3(0.f, 0.f, 0.f);
	XMFLOAT3 scale = XMFLOAT3(2.f, 2.f, 2.f);
	switch (weapontype)
	{
	case WEAPON_SWORD:
		
		scale = XMFLOAT3(1.f, 1.f, 1.f);
		break;
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

		rotate = XMFLOAT3(90.f, 0.f, 0.f);
		scale = XMFLOAT3(1.5f, 1.5f, 1.5f);
		break;
	case FLAG:
		modeltag = L"Component_Model_Flag";
		texturetag = L"Texture_Flag";

		rotate = XMFLOAT3(90.f, 0.f, 0.f);
		scale = XMFLOAT3(2.f, 2.f, 2.f);
		break;
	default:
		break;
	}
	m_pTransform->Set_Rotate(rotate.x, rotate.y, rotate.z);
	m_pTransform->Set_Scale(scale);

	m_pModel[m_AnimIndex] = GET_COMPONENT(LoadModel*, this, modeltag);
	pMaterial = GET_COMPONENT(CMaterial*, this, texturetag);
	DirtyFrames();
}

HRESULT CWeapon::Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
	HRESULT hr = E_FAIL;
	wstring modeltag = L"Component_Model_Sword";
	wstring texturetag = L"Texture_Sword_diffuse";
	hr = CModelObject::Initialize(modeltag, texturetag, pd3dDevice, pd3dCommandList);

	for (int i = WEAPON_HAMMER; i < TYPE_END; ++i)
	{
		switch (i)
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
		case FLAG:
			modeltag = L"Component_Model_Flag";
			texturetag = L"Texture_Flag";
			break;
		default:
			break;
		}

		Insert_Component_ToMap(modeltag);
		Insert_Component_ToMap(texturetag);
	}
	set_Type(m_type);
	if (FAILED(hr))
	{
		_MSG_BOX("CWeapon init Failed");
		return hr;
	}

	return S_OK;
}

int CWeapon::Update(float fTimeElapsed)
{
	if (m_bInvisible == true)
		return 0;

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

void CWeapon::Render(ID3D12GraphicsCommandList * pCommandList)
{
	if (m_bInvisible == true)
		return;

	CModelObject::Render(pCommandList);
}
