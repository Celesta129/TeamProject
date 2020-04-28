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
	void SetMesh(int nIndex, const shared_ptr<CMesh>& pMesh);

public:
	SkinnedConstants GetSkinnedConstants(void);

protected:
	LoadModel *m_model = nullptr;
	UINT m_NumofAnim = 0;
	UINT m_AnimIndex = -1;
	float m_Animtime = -1.f;
	UINT m_nMeshes = 0;

	vector<unique_ptr<CMesh>> m_ppMeshes;
	vector<XMFLOAT4X4> m_Bones;

};

