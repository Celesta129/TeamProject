#pragma once
#include "Model_TextureObject.h"
class CModel_AnimationObject :	public CModel_TextureObject
{
public:
	CModel_AnimationObject();
	virtual ~CModel_AnimationObject();


public:
	virtual HRESULT Initialize(void);
	virtual HRESULT Initialize(const wstring& modelTag, ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);

	virtual void Update(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pCommandList);

	void setMat(Material* pMaterial) { m_pMaterial = pMaterial; };
	Material* getMat(void) { return m_pMaterial; }
	Material* m_pMaterial = nullptr;
};

