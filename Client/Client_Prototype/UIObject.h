#pragma once
#include "ModelObject.h"
#include "Material.h"
class CMaterial;

class CUI_Timer :
	public CModelObject
{
public:
	CUI_Timer();
	virtual ~CUI_Timer();

public:
	virtual HRESULT Initialize(const wstring& texturetag, ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);

	virtual int Update(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pCommandList);

private:
	float m_fTimer = 10.f;
};

