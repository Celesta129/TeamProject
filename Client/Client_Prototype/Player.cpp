#include "Player.h"
#include "ModelObject.h"
#include "Component_Manager.h"
#include "Object_Manager.h"

#include "Weapon.h"
CPlayer::CPlayer()
{
	m_posX = 0.f;
	m_posY = 0.f;
	m_posZ = 0.f;

	m_velocityX = 0.f;
	m_velocityY = 0.f;
	m_velocityZ = 0.f;

	m_connected = false;
}


CPlayer::~CPlayer()
{
}

HRESULT CPlayer::Initialize(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	HRESULT hr = E_FAIL;
	hr = CModelObject::Initialize(L"Component_Model_idle", L"Texture_character", pd3dDevice, pd3dCommandList);
	if (FAILED(hr))
	{
		_MSG_BOX("Player init Failed");
		return hr;
	}

	AddModel(L"Component_Model_run");
	AddModel(L"Component_Model_attack");

	AddModel(L"Component_Model_hit");

	set_hand();
	return S_OK;
}

bool CPlayer::collision_weapon()
{
	vector<CGameObject*>* pvWeapons = m_pObject_Manager->Get_Layer(CObject_Manager::LAYER_WEAPON);
	for (UINT i = 0; i < pvWeapons->size(); ++i) {
		CWeapon* pWeapon = (CWeapon*)(*pvWeapons)[i];
		if (pWeapon->get_Invisible() == true)
			continue;

		if (pWeapon->get_Player())
			continue;

		CTransform* pWeaponTransform = pWeapon->Get_Transform();
		float dist = 0;

		XMFLOAT3 weaponPos = pWeaponTransform->Get_Pos();
		XMFLOAT3 pos = m_pTransform->Get_Pos();

		dist = sqrtf((weaponPos.x - pos.x) * (weaponPos.x - pos.x) +
			(weaponPos.z - pos.z) * (weaponPos.z - pos.z));

		if (dist <= 50.f)
		{
			pWeaponTransform->Set_Pos(XMFLOAT3(0.f, 0.f, -0.f));
			pWeaponTransform->Rotate(0.f, -90.f, 0.f);
			pWeapon->set_Player(this);
			
			cout << "collision_Weapon" << endl;
			return true;
		}
			
	}
	return false;
}

int CPlayer::Update(float fTimeElapsed)
{
	int result = UPDATE_NULL;
	
	result = CModelObject::Update(fTimeElapsed);

	bool handcheck = set_hand();
	// weapon �˻� �� collision üũ
	collision_weapon();

	return result;
}

void CPlayer::SetPos(float x, float y, float z)
{
	m_posX = x;
	m_posY = y;
	m_posZ = z;

	Get_Transform()->Set_Pos(XMFLOAT3(x, y, z));
	DirtyFrames();
}

void CPlayer::GetPos(float * x, float * y, float * z)
{
	*x = m_posX;
	*y = m_posY;
	*z = m_posZ;
}

void CPlayer::SetVelocity(float x, float y, float z)
{
	m_velocityX = x;
	m_velocityY = y;
	m_velocityZ = z;

	XMFLOAT3 velocity = XMFLOAT3(-m_velocityX, m_velocityY, -m_velocityZ);

	if (IsEqual(velocity.x, 0.0f) && IsEqual(velocity.z, 0.0f))
	{
		return;
	}
	m_pTransform->Set_Look(velocity);
	DirtyFrames();
}

void CPlayer::GetVelocity(float * x, float * y, float * z)
{
	*x = m_velocityX;
	*y = m_velocityY;
	*z = m_velocityZ;
}

void CPlayer::SetAnimation_index(int animation_index)
{
	m_animation_index = animation_index;

	ChangeAnim(animation_index);
}

void CPlayer::GetAnimation_index(int *animation_index)
{
	*animation_index = m_animation_index;
}

XMFLOAT4X4 CPlayer::get_Hand(void)
{
	XMFLOAT4X4 result;
	
	if (m_pmatHand != nullptr)
	{
		XMStoreFloat4x4(&result, XMMatrixTranspose(*m_pmatHand));		// ��ġ����̾����Ƿ� �ٽ� ����� �������ش�.
	}
	return result;
}

bool CPlayer::set_hand(void)
{
	m_pmatHand = &(m_pModel[m_AnimIndex]->GetBone("Bip001 L Hand")->FinalTransformation);
	
	if (!m_pmatHand)
		return false;
	
	return true;
}

