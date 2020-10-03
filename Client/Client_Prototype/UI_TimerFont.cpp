#include "UI_TimerFont.h"



CUI_TimerFont::CUI_TimerFont()
{
}


CUI_TimerFont::~CUI_TimerFont()
{
}

HRESULT CUI_TimerFont::Initialize(void)
{
	HRESULT hr = E_FAIL;

	if (FAILED(hr = CUI_Object::Initialize()))
		return hr;
	Add_Material(L"Texture_FlagTImeFont");
	Add_Material(L"Texture_LimitTImeFont");

	float Width = 280.f * 0.8f;
	float Height = 80.f * 0.8f;

	m_xmf2Pos = XMFLOAT2(FRAME_BUFFER_WIDTH * 0.4f, FRAME_BUFFER_WIDTH * 0.9f);
	m_xmf2Size = XMFLOAT2(Width, Height);

	return S_OK;
}
