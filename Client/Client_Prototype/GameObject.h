#pragma once
#include "stdafx.h"
#include "Base.h"
#include "../Common/UploadBuffer.h"
#include "LoadModel.h"
#include "FrameResource.h"		// For include struct ObjectConstant

#include "Component_Manager.h"
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
	CTransform* m_pTransform = nullptr;

public:
	CTransform* Get_Transform(void) {
		return m_pTransform;
	}

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
	map<wstring, CComponent*> m_mapComponent;
	CComponent_Manager* m_pComponent_Manager = nullptr;
};

