#pragma once
#include "stdafx.h"
#include "Base.h"
#include "../Common/UploadBuffer.h"
#include "LoadModel.h"
#include "FrameResource.h"		// For include struct ObjectConstant

class CComponent_Manager;

class CGameObject : public CBase
{
public:
	CGameObject();
	virtual ~CGameObject();

public:
	virtual HRESULT Initialize(void);

	virtual void Update(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* pCommandList);

	virtual int Free(void);
public:
	virtual ObjectConstants GetObjectConstants(void);	// �� ������Ʈ���� CB�� ���ε��� �ڷḦ ��ȯ�Ѵ�.
														// �ٸ� ObjectConstant�� Ŭ�������� ���Ҽ��� ������ �ڽ� Ŭ�������� �ʿ信 ���� ������ ����ü�� ������ ���� 
														// Ŭ���� ����� ����� ObjectConstants�� �����͸� ��ȯ�ϴ� ������ �Լ��� �ٲ㼭 ó������.

	// -----------------------for FramesDirty-----------------------
protected:
	int m_nNumFramesDirty = NUM_FRAME_RESOURCE;		// �� ���� 0���� ũ�ٸ� CB�� ������Ʈ�Ұ���.
public:
	int GetFramesDirty(void) { 
		return m_nNumFramesDirty; 
	}
	void DirtyFrames(void) {
		m_nNumFramesDirty = NUM_FRAME_RESOURCE;
	}
	void DecreaseFramesDirty(void) { 
		--m_nNumFramesDirty; 
	}
	// --------------------------------------------------------------

public:
	CComponent* Get_Component(const wstring tag);
	HRESULT Insert_Component_ToMap(const wstring tag);

	
protected:
	LoadModel* m_model;

protected:
	map<wstring, CComponent*> m_mapComponent;
	CComponent_Manager* m_pComponent_Manager = nullptr;
};

