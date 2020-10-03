#include "UI_Timer.h"



CUI_Timer::CUI_Timer()
{
}


CUI_Timer::~CUI_Timer()
{
}

HRESULT CUI_Timer::Initialize(void)
{
	HRESULT hr = E_FAIL;

	if (FAILED(hr = CUI_Object::Initialize()))
		return hr;
	Add_Material(L"Texture_Number");

	m_nNumIndex = 4;

	float Width = 48.f * 4.f;
	float Height = 48.f;

	m_xmf2Pos = XMFLOAT2(FRAME_BUFFER_WIDTH * 0.5f, FRAME_BUFFER_HEIGHT * 0.95f);
	m_xmf2Size = XMFLOAT2(Width, Height);

	return S_OK;
}

void CUI_Timer::Set_Time(int time)
{
	m_fTime = time;
}

UI_Constants CUI_Timer::Get_UIConstants(void)
{
	UI_Constants result = CUI_Object::Get_UIConstants();
	result.data = m_fTime;

	return result;
}

int CUI_Timer::Update(float fTimeElapsed)
{
	DirtyFrames();
	return CUI_Object::Update(fTimeElapsed);
}
