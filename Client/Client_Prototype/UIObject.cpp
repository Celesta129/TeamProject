#include "UIObject.h"



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
	//CModelObject::Render(pCommandList);
}
