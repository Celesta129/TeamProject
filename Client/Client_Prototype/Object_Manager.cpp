#include "Object_Manager.h"

_IMPLEMENT_SINGLETON(CObject_Manager)

CObject_Manager::CObject_Manager()
{

}

void CObject_Manager::Add_Object(UINT layer, CGameObject * pObject)
{
	m_vObject[layer].push_back(pObject);
}

CGameObject * CObject_Manager::Get_Object(UINT layer, UINT index)
{
	CGameObject* result = nullptr;
	if (layer >= LAYER_END)
		return nullptr;
	if (m_vObject[layer].size() < index)
		return nullptr;

	result = m_vObject[layer][index];
	return result;
}

vector<CGameObject*>* CObject_Manager::Get_Layer(UINT layer)
{
	if(layer >= LAYER_END)
		return nullptr;

	return &m_vObject[layer];
}

int CObject_Manager::Free(void)
{
	int ref = 0;
	for (UINT i = 0; i < LAYER_END; ++i)
	{
		for (auto& object : m_vObject[i])
		{
			ref = Safe_Release(object);
		}
	}
	return 0;
}
