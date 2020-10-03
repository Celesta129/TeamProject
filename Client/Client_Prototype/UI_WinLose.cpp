#include "UI_WinLose.h"



CUI_WinLose::CUI_WinLose()
{
}


CUI_WinLose::~CUI_WinLose()
{
}

HRESULT CUI_WinLose::Initialize(void)
{
	HRESULT hr = E_FAIL;

	if (FAILED(hr = CUI_Object::Initialize()))
		return hr;
	Add_Material(L"Texture_Win");
	Add_Material(L"Texture_Lose");


	float Width = 640 * 0.5f;
	float Height = 640 * 0.5f;

	m_xmf2Pos = XMFLOAT2(FRAME_BUFFER_WIDTH * 0.5f, FRAME_BUFFER_HEIGHT * 0.5f);
	m_xmf2Size = XMFLOAT2(Width, Height);

	return S_OK;
}

int CUI_WinLose::Update(float fTimerElapsed)
{
	if (m_bInvisible == false)
	{
		m_fTimer -= fTimerElapsed;
	}
	if (m_fTimer < 0.f) {
		m_fTimer = 5.f;
		m_bInvisible = true;
	}
	return 0;
}
