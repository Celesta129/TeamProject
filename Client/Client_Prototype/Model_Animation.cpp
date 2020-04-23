#include "Model_Animation.h"



Model_Animation::Model_Animation()
{
}

Model_Animation::Model_Animation(const string Model_filename, vector<pair<string, float>>* Animation_filename)
{
	UINT animCount;
	UINT index;
	m_pModel = new LoadModel(Model_filename);
	m_vAnimStack = new vector<LoadAnimation*>; //애니메이션을 적재할 벡터 생성

	animCount = Animation_filename->size();
	m_vAnimStack->reserve(animCount);

	for (UINT i = 0; i < animCount; ++i) {
		LoadAnimation* Anim = new LoadAnimation((Animation_filename->begin() + i)->first, (Animation_filename->begin() + i)->second, 0); //애니메이션을 로딩
		m_vAnimStack->push_back(Anim); //애니메이션을 벡터에 적재
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
	// 복사생성하는걸로 수정해야할듯.
	AddRef();
	return this;
}
