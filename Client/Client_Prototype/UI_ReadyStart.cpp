#include "UI_ReadyStart.h"



CUI_ReadyStart::CUI_ReadyStart()
{
}


CUI_ReadyStart::~CUI_ReadyStart()
{
}

HRESULT CUI_ReadyStart::Initialize(void)
{
	HRESULT hr = E_FAIL;

	if (FAILED(hr = CUI_Object::Initialize()))
		return hr;
	Add_Material(L"Texture_Ready");
	Add_Material(L"Texture_Start");


	float width = 280.f;
	float height = 68.f;
	m_xmf2Pos = XMFLOAT2(FRAME_BUFFER_WIDTH / 2, FRAME_BUFFER_HEIGHT / 2);
	m_xmf2Size = XMFLOAT2(width , height);

	return S_OK;
}
