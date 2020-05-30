#pragma once
#include "GameObject.h"
#include "Material.h"

class CMaterial;
class CModelObject : public CGameObject
{
public:
	CModelObject();
	virtual ~CModelObject();
public:
	virtual HRESULT Initialize(void);
	virtual HRESULT Initialize(const wstring& modeltag,const wstring& texturetag, ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);

	virtual int Update(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pCommandList);

public:
	SkinnedConstants GetSkinnedConstants(void);

	CMaterial* GetMaterial(void) {
		return pMaterial;
	};
public:
	void ChangeAnim(const UINT& index);

	void setAnimIndex(const UINT& index);
	UINT getAnimIndex(void);

	void setMat(Material* pMaterial) { m_pMaterial = pMaterial; };
	void AddModel(const wstring& pModel);
	
	Material* getMat(void) { return m_pMaterial; }
	UINT getAnimStatus(void) {
		return m_AnimStatus;
	};

protected:
	void Animate(const float fTimeElapsed);
protected:
	vector<LoadModel*>m_pModel;
	UINT m_NumofAnim = 0;
	UINT m_AnimIndex = -1;
	float m_Animtime = -1.f;
	UINT m_nMeshes = 0;

	UINT m_AnimStatus;

	vector<XMFLOAT4X4> m_Bones;
	Material* m_pMaterial = nullptr;

	CMaterial* pMaterial = nullptr;
};

