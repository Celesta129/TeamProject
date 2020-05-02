#pragma once
#include "GameObject.h"

class CModelObject : public CGameObject
{
public:
	CModelObject();
	virtual ~CModelObject();
public:
	virtual HRESULT Initialize(void);
	virtual HRESULT Initialize(const wstring& tag, ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);

	virtual void Update(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pCommandList);

public:
	SkinnedConstants GetSkinnedConstants(void);

public:
	void setMat(Material* pMaterial) { m_pMaterial = pMaterial; };
	void AddModel(LoadModel* pModel);
	
	Material* getMat(void) { return m_pMaterial; }
	
protected:
	void Animate(const float fTimeElapsed);
protected:
	vector<LoadModel*>m_pModel;
	UINT m_NumofAnim = 0;
	UINT m_AnimIndex = -1;
	float m_Animtime = -1.f;
	UINT m_nMeshes = 0;

	vector<XMFLOAT4X4> m_Bones;
	Material* m_pMaterial = nullptr;
};

