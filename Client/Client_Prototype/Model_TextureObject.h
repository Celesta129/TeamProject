#pragma once
#include "ModelObject.h"
class CModel_TextureObject : public CModelObject
{
public:
	CModel_TextureObject();
	virtual ~CModel_TextureObject();

public:
	virtual HRESULT Initialize(void);
	virtual HRESULT Initialize(const wstring& modelTag, ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);

	virtual void Update(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pCommandList);

	void setMat(Material* pMaterial) { m_pMaterial = pMaterial; };
	Material* getMat(void) { return m_pMaterial; }
	Material* m_pMaterial = nullptr;


	UINT  m_currAnimIndex = 0;
	float fTime = 0.0f;

private:
	void Animate(const float fTimeElapsed);
};

