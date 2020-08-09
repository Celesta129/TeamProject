#include "UIObject.h"
#include "Player.h"


CUI_Object::CUI_Object()
{
}

CUI_Object::~CUI_Object()
{
}

HRESULT CUI_Object::Initialize(void)
{
	HRESULT hr = E_FAIL;

	if (FAILED(hr = CGameObject::Initialize()))
		return hr;

	return S_OK;
}

HRESULT CUI_Object::Initialize(const wstring & texturetag)
{
	HRESULT hr = E_FAIL;

	if (FAILED(hr = CGameObject::Initialize()))
		return hr;
	if (FAILED(hr = Insert_Component_ToMap(texturetag)))
		return hr;

	pMaterial.push_back((CMaterial*)Get_Component(texturetag));
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
	if (!m_bInvisible)
	{
		pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pCommandList->DrawInstanced(6 * m_nNumIndex, 1, 0, 0);
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
	UI_Constants result;

	result.xmf2ScreenPos = m_xmf2Pos;
	result.xmf2UISize = m_xmf2Size;
	result.xmf2Scale = XMFLOAT2(1.f, 1.f);
	result.data = 1.f;
	result.alpha = 1.f;

	return result;
}

void CUI_Object::SetMaterialIndex(UINT index)
{
	 m_nCurrMatIndex = index;
}

CMaterial * CUI_Object::GetMaterial(void)
{
	if (pMaterial.empty() || pMaterial.size() < m_nCurrMatIndex)
		return nullptr;

	return pMaterial[m_nCurrMatIndex];
}

void CUI_Object::Add_Material(const wstring & texturetag)
{
	Insert_Component_ToMap(texturetag);

	pMaterial.push_back((CMaterial*)Get_Component(texturetag));
}

