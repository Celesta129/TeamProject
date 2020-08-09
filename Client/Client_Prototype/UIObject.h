#pragma once
#include "ModelObject.h"
#include "Material.h"
class CMaterial;
class CPlayer;

class CUI_Object 
	: public CGameObject
{
public:
	CUI_Object();
	~CUI_Object();
public:
	virtual HRESULT Initialize(void);
	virtual HRESULT Initialize(const wstring& texturetag);

	virtual int Update(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pCommandList);

public:
	void SetPos(XMFLOAT2 pos);
	void SetSize(XMFLOAT2 size);

	void SetInvisible(bool invisible);

	XMFLOAT2 GetPos(void);
	XMFLOAT2 GetSize(void);

	virtual UI_Constants Get_UIConstants(void);

	void SetMaterialIndex(UINT index);
	CMaterial* GetMaterial(void);
	vector<CMaterial*>* GetAllMaterial(void) {
		return &pMaterial;
	};

	virtual void SetTarget(CGameObject* pTarget) {};
protected:
	void Add_Material(const wstring& texturetag);

	XMFLOAT2 m_xmf2Pos = XMFLOAT2(0.f, 0.f);		// pos는 y가 아래서부터 0임.(셰이더코드)
	XMFLOAT2 m_xmf2Size = XMFLOAT2(100.f, 100.f);

	vector<CMaterial*> pMaterial;
	UINT m_nCurrMatIndex = 0;

	bool m_bInvisible = true;
};


