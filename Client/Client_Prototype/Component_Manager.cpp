#include "Component_Manager.h"

_IMPLEMENT_SINGLETON(CComponent_Manager)

CComponent_Manager::CComponent_Manager()
{

}

void CComponent_Manager::Add_Component(const wstring & tag, CComponent * pComponent)
{
	m_mapComponent.insert(make_pair(tag, pComponent));
}

CComponent * CComponent_Manager::Clone_Component(const wstring & tag)
{
	CComponent* find_Result = Find_Component(tag);
	if (find_Result == nullptr)
		return nullptr;
	return find_Result->Clone();
}

void CComponent_Manager::Clear_Component(void)
{
	for (auto& e : m_mapComponent)
	{
		Safe_Release(e.second);
	}
	m_mapComponent.clear();
}

CComponent * CComponent_Manager::Find_Component(const wstring & tag)
{
	auto iter = find_if(m_mapComponent.begin(), m_mapComponent.end(), Finder_Tag(tag));

	if (iter == m_mapComponent.end())
		return nullptr;

	return iter->second;
}

int CComponent_Manager::Free(void)
{
	int	RefCnt = 0;

	for (auto& Pair : m_mapComponent)
	{
		if (RefCnt = Safe_Release(Pair.second))
			return RefCnt;
	}
	m_mapComponent.clear();

	return RefCnt;
}
