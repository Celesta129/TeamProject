#include "Model_Animation.h"



Model_Animation::Model_Animation()
{
}

Model_Animation::Model_Animation(const string Model_filename, vector<pair<string, float>>* Animation_filename)
{
	UINT animCount;
	UINT index;
	m_pModel = new LoadModel(Model_filename);
	m_vAnimStack = new vector<LoadAnimation*>; //�ִϸ��̼��� ������ ���� ����

	animCount = Animation_filename->size();
	m_vAnimStack->reserve(animCount);

	for (UINT i = 0; i < animCount; ++i) {
		LoadAnimation* Anim = new LoadAnimation((Animation_filename->begin() + i)->first, (Animation_filename->begin() + i)->second, 0); //�ִϸ��̼��� �ε�
		m_vAnimStack->push_back(Anim); //�ִϸ��̼��� ���Ϳ� ����
	}
}


Model_Animation::~Model_Animation()
{
}

void Model_Animation::LoadingModels(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
}

CComponent * Model_Animation::Clone(void)
{
	// ��������ϴ°ɷ� �����ؾ��ҵ�.
	AddRef();
	return this;
}
