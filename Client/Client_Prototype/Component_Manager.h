#pragma once
#include "stdafx.h"
#include "Component.h"

// �� ������Ʈ�� ������ �����մϴ�. �ʿ��� Ŭ�������� ���⼭ �����ؼ� ���پ��ϴ�. (Clone_Component)
class CComponent_Manager
{
	_DECLARE_SINGLETON(CComponent_Manager)
public:
	explicit CComponent_Manager();
	virtual ~CComponent_Manager() = default;
public:
	void Add_Component(const wstring& tag, CComponent* pComponent);
	CComponent* Clone_Component(const wstring& tag);
	void Clear_Component(void);

private:
	CComponent* Find_Component(const wstring& tag);

	void Release(void);
private:
	map<const wstring, CComponent*> m_mapComponent;
};

