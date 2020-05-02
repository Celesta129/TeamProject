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
	virtual ObjectConstants GetObjectConstants(void);	// 각 오브젝트들의 CB에 업로드할 자료를 반환한다.
														// 다만 ObjectConstant는 클래스마다 변할수도 있으니 자식 클래스에서 필요에 따라 수정된 구조체를 정의한 다음 
														// 클래스 멤버로 만들고 ObjectConstants의 포인터를 반환하는 식으로 함수를 바꿔서 처리하자.
	
	// -----------------------for FramesDirty-----------------------
protected:
	int m_nNumFramesDirty = NUM_FRAME_RESOURCE;		// 이 수가 0보다 크다면 CB를 업데이트할것임.
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
	map<wstring, CComponent*> m_mapComponent;
	CComponent_Manager* m_pComponent_Manager = nullptr;
};

