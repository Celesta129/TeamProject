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
	CMaterial* GetMaterial(void);

	virtual void SetTarget(CGameObject* pTarget) {};
protected:
	XMFLOAT2 m_xmf2Pos = XMFLOAT2(0.f, 0.f);
	XMFLOAT2 m_xmf2Size = XMFLOAT2(100.f, 100.f);

	CMaterial* pMaterial = nullptr;

	bool m_bInvisible = true;
};

class CUI_HPBar
	: public CUI_Object
{
public:
	CUI_HPBar();
	~CUI_HPBar();
public:
	virtual HRESULT Initialize(void);

	virtual int Update(float fTimeElapsed);
	//virtual void Render(ID3D12GraphicsCommandList* pCommandList);

	virtual UI_Constants Get_UIConstants(void);

public:
	virtual void SetTarget(CGameObject* pTarget);
private:
	CPlayer* m_pTarget = nullptr;
	float m_fHP = 0.f;
};

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

