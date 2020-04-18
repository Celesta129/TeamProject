#include "Model_Animation.h"



Model_Animation::Model_Animation()
{
}


Model_Animation::~Model_Animation()
{
}

LoadAnimation::LoadAnimation(string filename, float trigger, float skip)
{
	m_pScene = aiImportFile(filename.c_str(), (aiProcessPreset_TargetRealtime_Quality | aiProcess_ConvertToLeftHanded) & ~aiProcess_FindInvalidData);
	if (m_pScene) {
		m_pAnim = m_pScene->mAnimations[0]; //���� �ִϸ��̼Ǹ� ����ϴ� ��� 0�� �ε���
		//m_GlobalInverse = aiMatrixToXMMatrix(m_pScene->mRootNode->mTransformation);
		m_GlobalInverse = XMMatrixIdentity();

		start_time = (float)m_pAnim->mChannels[0]->mPositionKeys[0].mTime;
		//������ ���� ������ ��ǥ �̵� �������� �������� ����
		end_time = (float)m_pAnim->mChannels[0]->mPositionKeys[m_pAnim->mChannels[0]->mNumPositionKeys - 1].mTime - 1.0f;
		//������ ���� �������� 1.0 ��ŭ ����� �������� �Ȱ�ħ

		//cout << filename << " start Time : " << start_time << endl;
		//cout << filename << " end Time : " << end_time << endl;

		if (IsZero(trigger))
			trigger_time = (end_time - start_time) / 2 + start_time;
		else
			trigger_time = trigger + start_time;

		if (IsZero(skip))
			posible_skip = end_time;
		else
			posible_skip = skip + start_time;

		now_time = start_time;

		m_fAnimSpeed = (end_time - start_time) / m_pAnim->mChannels[0]->mNumPositionKeys;
		m_fAnimSpeed = m_pAnim->mTicksPerSecond;
		m_fInitSpeed = m_fAnimSpeed;
	}

}

LoadAnimation::LoadAnimation(const LoadAnimation & T)
{
	m_pScene = T.m_pScene;
	m_pAnim = T.m_pAnim;
	m_GlobalInverse = XMMatrixIdentity();
	start_time = T.start_time;
	end_time = T.end_time;
	now_time = T.now_time;
	trigger_time = T.trigger_time;
	posible_skip = T.posible_skip;
	m_fAnimSpeed = T.m_fAnimSpeed;
	m_fPrevSpeed = T.m_fPrevSpeed;
	m_fInitSpeed = T.m_fInitSpeed;
}

LoadAnimation::~LoadAnimation()
{
}

UINT LoadAnimation::BoneTransform(UINT & index, float ftime, vector<XMFLOAT4>& transforms)
{
	return 0;
}

void LoadAnimation::ReadNodeHeirarchy(float fAnimTime, const aiNode * pNode, const XMMATRIX & ParentTransform)
{
}

const aiNodeAnim * LoadAnimation::FindNodeAnim(const aiAnimation * pAnimation, const string & NodeName)
{
	return nullptr;
}

void LoadAnimation::CalcInterpolatedScaling(aiVector3D & Out, float AnimationTime, const aiNodeAnim * pNodeAnim)
{
}

void LoadAnimation::CalcInterpolatedRotation(aiQuaternion & Out, float AnimationTime, const aiNodeAnim * pNodeAnim)
{
}

void LoadAnimation::CalcInterpolatedPosition(aiVector3D & Out, float AnimationTime, const aiNodeAnim * pNodeAnim)
{
}

unsigned int LoadAnimation::FindScaling(float AnimationTime, const aiNodeAnim * pNodeAnim)
{
	return 0;
}

unsigned int LoadAnimation::FindRotation(float AnimationTime, const aiNodeAnim * pNodeAnim)
{
	return 0;
}

unsigned int LoadAnimation::FindPosition(float AnimationTime, const aiNodeAnim * pNodeAnim)
{
	return 0;
}

void LoadAnimation::SetAnimFrame(float frame)
{
}
