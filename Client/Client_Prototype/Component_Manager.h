#pragma once
#include "stdafx.h"
#include "Base.h"
#include "Component.h"

#include "Transform.h"
#include "LoadModel.h"

// 각 컴포넌트의 원형을 보관합니다. 필요한 클래스들은 여기서 복사해서 갖다씁니다. (Clone_Component)
class CComponent_Manager final : public CBase
{
	_DECLARE_SINGLETON(CComponent_Manager)
public:
	explicit CComponent_Manager();
	virtual ~CComponent_Manager() = default;
public:
	void Add_Component(const wstring& tag, CComponent* pComponent);
	CComponent* Clone_Component(const wstring& tag);

	void Clear_Component(void);

	map<const wstring, CComponent*>* Get_Map(void) { return &m_mapComponent; }
private:
	CComponent* Find_Component(const wstring& tag);
private:
	map<const wstring, CComponent*> m_mapComponent;

	virtual int Free(void);
};

