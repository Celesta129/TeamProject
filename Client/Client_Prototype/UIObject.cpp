#include "UIObject.h"
#include "Player.h"


CUI_Object::CUI_Object()
{
}

CUI_Object::~CUI_Object()
{
}

HRESULT CUI_Object::Initialize(const wstring & texturetag)
{
	HRESULT hr = E_FAIL;

	if (FAILED(hr = CGameObject::Initialize()))
		return hr;
	if (FAILED(hr = Insert_Component_ToMap(texturetag)))
		return hr;

	pMaterial = (CMaterial*)Get_Component(texturetag);
	return S_OK;
}

int CUI_Object::Update(float fTimeElapsed)
{
	CGameObject::Update(fTimeElapsed);

	return 0;
}

void CUI_Object::Render(ID3D12GraphicsCommandList * pCommandList)
{
	//pd3dCommandList->SetGraphicsRootDescriptorTable(0, m_d3dCbvGPUDescriptorHandle);
	if (m_bInvisible)
	{
		pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pCommandList->DrawInstanced(6, 1, 0, 0);
	}
}

void CUI_Object::SetPos(XMFLOAT2 pos)
{
	m_xmf2Pos = pos;
}

void CUI_Object::SetSize(XMFLOAT2 size)
{
	m_xmf2Size = size;
}

void CUI_Object::SetInvisible(bool invisible)
{
	m_bInvisible = invisible;
}

XMFLOAT2 CUI_Object::GetPos(void)
{
	return m_xmf2Pos;
}

XMFLOAT2 CUI_Object::GetSize(void)
{
	return m_xmf2Size;
}

UI_Constants CUI_Object::Get_UIConstants(void)
{
	return UI_Constants();
}

CMaterial * CUI_Object::GetMaterial(void)
{
	return pMaterial;
}


CUI_Timer::CUI_Timer()
{
}


CUI_Timer::~CUI_Timer()
{
}

HRESULT CUI_Timer::Initialize(const wstring & texturetag, ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
	HRESULT hr = E_FAIL;
	const wstring& modeltag = L"Component_Model_UIPlane";

	if (FAILED(hr = CGameObject::Initialize()))
		return hr;
	if (FAILED(hr = Insert_Component_ToMap(modeltag)))
		return hr;
	if (FAILED(hr = Insert_Component_ToMap(texturetag)))
		return hr;
	m_pModel.push_back((LoadModel*)Get_Component(modeltag));
	pMaterial = (CMaterial*)Get_Component(texturetag);

	//매쉬 적용
	for (UINT i = 0; i < m_nMeshes; ++i) {
		if (i > 0)
			m_pModel[m_AnimIndex]->SetTextureIndex(i, i);
	}

	m_pTransform->Set_Pos(XMFLOAT3(0.f, 0.f, 100.f));
	m_pTransform->Set_Scale(XMFLOAT3(150, 150.f, 1.f));
	return S_OK;
}

int CUI_Timer::Update(float fTimeElapsed)
{
	m_fTimer -= fTimeElapsed;
	if (m_fTimer < 0.f)
	{
		cout << "timer out" << endl;
		m_fTimer = 30.f;
		return UPDATE_TIMEOUT;
	}
		

	return UPDATE_NULL;
}

void CUI_Timer::Render(ID3D12GraphicsCommandList * pCommandList)
{
	CModelObject::Render(pCommandList);
	
	//pd3dCommandList->SetGraphicsRootDescriptorTable(0, m_d3dCbvGPUDescriptorHandle);

	pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pCommandList->DrawInstanced(6, 1, 0, 0);
}

CUI_HPBar::CUI_HPBar()
{
}

CUI_HPBar::~CUI_HPBar()
{
}

HRESULT CUI_HPBar::Initialize(void)
{
	HRESULT hr = E_FAIL;

	if(FAILED(hr = CUI_Object::Initialize(L"Texture_HPbar")))
		return hr;

	m_xmf2Pos = XMFLOAT2(400.f, 400.f);
	m_xmf2Size = XMFLOAT2(100.f, 33.f);

	return S_OK;
}

int CUI_HPBar::Update(float fTimeElapsed)
{
	if (m_pTarget != nullptr)
	{
		m_fHP = m_pTarget->Get_HP();
		DirtyFrames();
	}
	return 0;
}

UI_Constants CUI_HPBar::Get_UIConstants(void)
{
	UI_Constants result;
	
	result.xmf2ScreenPos = m_xmf2Pos;
	result.xmf2UISize = m_xmf2Size;
	result.xmf2Scale = XMFLOAT2(1.f, 1.f);
	result.data = m_fHP;
	result.alpha = 1.f;
	
	return result;
}

void CUI_HPBar::SetTarget(CGameObject * pTarget)
{
	m_pTarget = (CPlayer*)pTarget;
}
