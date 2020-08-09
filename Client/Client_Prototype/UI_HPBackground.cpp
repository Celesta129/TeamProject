#include "UI_HPBackground.h"



CUI_HPBackground::CUI_HPBackground()
{
}


CUI_HPBackground::~CUI_HPBackground()
{
}

HRESULT CUI_HPBackground::Initialize(void)
{
	HRESULT hr = E_FAIL;

	if (FAILED(hr = CUI_Object::Initialize()))
		return hr;
	Add_Material(L"Texture_HpEdge1");
	Add_Material(L"Texture_HpEdge2");
	Add_Material(L"Texture_HpEdge3");
	Add_Material(L"Texture_HpEdge4");
	Add_Material(L"Texture_HpEdge5");
	Add_Material(L"Texture_HpEdge6");
	Add_Material(L"Texture_HpEdge7");
	Add_Material(L"Texture_HpEdge8");

	float hpEdge_Width = 320.f * 0.8f;
	float hpEdge_Height = 80.f * 0.8f;

	m_xmf2Pos = XMFLOAT2(400.f, 400.f);
	m_xmf2Size = XMFLOAT2(hpEdge_Width, hpEdge_Height);

	return S_OK;
}
