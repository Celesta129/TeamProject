#include "UI_HPBar.h"
#include "Player.h"


CUI_HPBar::CUI_HPBar()
{
}


CUI_HPBar::~CUI_HPBar()
{
}

HRESULT CUI_HPBar::Initialize(void)
{
	HRESULT hr = E_FAIL;

	if (FAILED(hr = CUI_Object::Initialize(L"Texture_HPbar")))
		return hr;


	float hpBar_Width = 290.f * 0.8f;
	float hpBar_Height = 44.f * 0.8f;
	m_xmf2Pos = XMFLOAT2(800, 400);
	m_xmf2Size = XMFLOAT2(hpBar_Width, hpBar_Height);

	return S_OK;
}

int CUI_HPBar::Update(float fTimeElapsed)
{
	if (m_pTarget != nullptr)
	{
		if (m_pTarget->m_connected)
		{
			m_fHP = m_pTarget->Get_HP();
			DirtyFrames();
		}
	}
	return 0;
}

UI_Constants CUI_HPBar::Get_UIConstants(void)
{
	UI_Constants result;

	result.xmf2ScreenPos = m_xmf2Pos;
	result.xmf2UISize = m_xmf2Size;
	result.xmf2Scale = XMFLOAT2(1.f, 1.f);
	if (m_pTarget)
		result.data = m_fHP * 0.01f;
	else
		result.data = 0.f;
	result.alpha = 1.f;

	return result;
}

void CUI_HPBar::SetTarget(CGameObject * pTarget)
{
	m_pTarget = (CPlayer*)pTarget;
}
