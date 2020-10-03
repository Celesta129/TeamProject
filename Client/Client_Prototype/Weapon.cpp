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
			pos = XMFLOAT3(-36.f, -32.f, -24.f);
			rotate = XMFLOAT3(63.f, -155.f, 90.f);
			scale = XMFLOAT3(0.5f, 0.5f, 0.5f);
			break;
		case WEAPON_HAMMER:
			pos = XMFLOAT3(-29.f, -1.5f, -12.0f);
			rotate = XMFLOAT3(-100.f, 200.f, 160.f);
			scale = XMFLOAT3(1.f, 1.f, 1.f);
			break;
		case WEAPON_BLOCK:

			break;
		case WEAPON_SNACK:
			pos = XMFLOAT3(-26.f, -9.f, -4.5f);
			rotate = XMFLOAT3(0.f, 0.f, 260.f);
			scale = XMFLOAT3(0.5f, 0.5f, 0.5f);
			break;
		case FLAG:
			pos = XMFLOAT3(0.f, 8.f, 0.f);
			rotate = XMFLOAT3(3.f, 0.f, 30.f);
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
	pMaterial[0] = GET_COMPONENT(CMaterial*, this, texturetag);
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

	
	if (m_pPlayer)
	{
		//hand¸¦ Ã£ÀÚ
		XMFLOAT4X4 hand = m_pPlayer->get_Hand();
		XMFLOAT4X4 spine = m_pPlayer->get_Spine();
		XMFLOAT4X4 playerWorld = m_pPlayer->Get_Transform()->Get_World();

		switch (m_type)
		{
		case FLAG:
			m_matParent = Matrix4x4::Multiply(spine, playerWorld);
			break;
		default:
			m_matParent = Matrix4x4::Multiply(hand, playerWorld);
			break;
		}
		

		m_pTransform->Set_Parent(&m_matParent);

		XMFLOAT4X4 world = m_pTransform->Get_World();


		float fPower = 30.f;
		if(GetAsyncKeyState(VK_RETURN) & 0x8000)
		{
			cout << m_pTransform->Get_Rotate().x << ", " << m_pTransform->Get_Rotate().y<< ", " << m_pTransform->Get_Rotate().z << endl;
			cout << m_pTransform->Get_Pos().x << ", " << m_pTransform->Get_Pos().y << ", " << m_pTransform->Get_Pos().z << endl;
		}
		XMFLOAT3 rotation = XMFLOAT3(fPower * fTimeElapsed, fPower * fTimeElapsed, fPower * fTimeElapsed);

		if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
			rotation = XMFLOAT3(-fPower * fTimeElapsed, -fPower * fTimeElapsed, -fPower * fTimeElapsed);

		if (GetAsyncKeyState('X') & 0x8000)
		{
			m_pTransform->Rotate(rotation.x, 0.f, 0.f);
		}
		if (GetAsyncKeyState('Y') & 0x8000)
		{
			m_pTransform->Rotate(0.f, rotation.y, 0.f);
		}
		if (GetAsyncKeyState('Z') & 0x8000)
		{
			m_pTransform->Rotate(0.f, 0.f, rotation.z);
		}

		if (GetAsyncKeyState('1') & 0x8000)
		{
			m_pTransform->MovePos(XMFLOAT3(rotation.x, 0.f, 0.f));
		}
		if (GetAsyncKeyState('2') & 0x8000)
		{
			m_pTransform->MovePos(XMFLOAT3(0.f, rotation.y, 0.f));
		}
		if (GetAsyncKeyState('3') & 0x8000)
		{
			m_pTransform->MovePos(XMFLOAT3(0.f, 0.f, rotation.z));
		}
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
