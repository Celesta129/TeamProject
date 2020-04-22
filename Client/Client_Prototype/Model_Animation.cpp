#include "Model_Animation.h"


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
		//������ ���� �������� 1.0 ��ŭ ����� �������� �Ȱ�ħ	-> ��?

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
		m_fAnimSpeed = (float)m_pAnim->mTicksPerSecond;
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
	if (m_pScene)
		m_pScene->~aiScene();

	if (m_Bones.size() > 0) {
		vector<pair<string, Bone>> a;
		m_Bones.swap(a);
		a.clear();
		m_Bones.clear();
		//		cout << "Loadanimation m_bones clear" << endl;
	}
}

UINT LoadAnimation::BoneTransform(UINT & index, float fTime, vector<XMFLOAT4X4>& transforms)
{
	if (stop_anim)
		return LOOP_STOP;
	XMMATRIX Identity = XMMatrixIdentity();

	if (!m_pScene) {
		//�ִϸ��̼� ������ �ε�� ���� ��� ����
		for (UINT i = 0; i < m_NumBones; ++i) {
			XMStoreFloat4x4(&transforms[i], Identity);
		}
		return LOOP_IN;
	}

	////��Ʈ������ ���������� �m��� ��ȯ ���� �� ���� ������ȯ ���
	ReadNodeHeirarchy(now_time, m_pScene->mRootNode, Identity);

	for (int i = 0; i < m_NumBones; ++i) {
		//���� ������ȯ�� ��ȯ
		XMStoreFloat4x4(&transforms[i], m_Bones[i].second.FinalTransformation);

		//if (m_Bones[i].first == "Bip001 L Hand") {
		//	//XMVECTOR tmp = p.second.BoneOffset.r[3];
		//	//cout << "Left Hand BondeOffset: " << tmp.m128_f32[0] << "," << tmp.m128_f32[1] << "," << tmp.m128_f32[2] << endl;
		//	//
		//	//tmp = p.second.FinalTransformation.r[3];
		//	//cout << "Left Hand FinalTransformation: " << tmp.m128_f32[0] << "," << tmp.m128_f32[1] << "," << tmp.m128_f32[2] << endl;

		//	XMMATRIX tmp[2];
		//	tmp[0] = m_Bones[14].second.FinalTransformation;
		//	tmp[1] = m_Bones[16].second.FinalTransformation;

		//	XMFLOAT4 pos = XMFLOAT4(0, 0, 0, 1);
		//	float weights[2] = { 0.25f, 0.75f };

		//	//for (int j = 0; j < 4; j++) {
		//	//	weights[j] = 0.25f;
		//	//}

		////	XMFLOAT4 temp = XMFLOAT4(33.7235756, 38.8664284, -3.53839922, 1);
		//	XMFLOAT4 temp = XMFLOAT4(31.7235756, 35.8664284, -3.53839922, 1);
		//	XMFLOAT4 result;
		//	for (int j = 0; j < 2; j++)
		//	{
		//		result = Matrix4x4::test(temp, tmp[j]);
		//		pos.x += weights[j] * result.x;
		//		pos.y += weights[j] * result.y;
		//		pos.z += weights[j] * result.z;
		//	}
		//	//cout << "Left Hand Pos : " << pos.x << "," << pos.y << "," << pos.z << endl;

		//	//tmp2._41 = pos.x;
		//	//tmp2._42 = pos.y;
		//	//tmp2._43 = pos.z;

		//	//pos = Matrix4x4::test(pos, WORLDMATRIX);

		//	m_handpos = XMFLOAT4(pos.x, pos.y, pos.z, 1);

		//	int k = 0;
		//}
	}

	//// �̸� ������ ������ ������ �ִϸ��̼� ����
	now_time += m_fAnimSpeed * fTime;
	/*if (index == Anim_Guard || index == Anim_chocolate_Guard || index == Anim_Lollipop_Guard)
	{
		;
	}
	else now_time += m_animSpeed * fTime;*/
	if (now_time > end_time) {
		now_time = start_time;
		if (!animation_Loop) {
			index = Anim_Idle;
			animation_Loop = true;
		}
		return LOOP_END; //�ִϸ��̼��� �� ���� ����
	}

	////cout << m_animSpeed << ", " << fTime << endl;
	////cout << "���� : " << now_time << ", " << "�� �ð� : " << end_time << endl;

	if (now_time > trigger_time - 1 && now_time < trigger_time + 1) {
		return LOOP_TRIGGER;
	}

	if (now_time > posible_skip)
		return LOOP_SKIP; //�ִϸ��̼� ���� �������̰� Ʈ���� ���� ���Ĺݺ�

	return LOOP_IN; //�ִϸ��̼��� ���� ������
}

void LoadAnimation::ReadNodeHeirarchy(float fAnimTime, const aiNode * pNode, const XMMATRIX & ParentTransform)
{
	const aiAnimation* pAnim = m_pAnim;

	XMMATRIX NodeTransformation = aiMatrixToXMMatrix(pNode->mTransformation);

	const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnim, pNode->mName.data);
	//���� ��尡 animation channel �� ������ Ȯ��
	if (pNodeAnim) {
		aiVector3D s;
		CalcInterpolatedScaling(s, fAnimTime, pNodeAnim);
		XMMATRIX ScalingM = XMMatrixScaling(s.x, s.y, s.z);


		aiQuaternion q;
		CalcInterpolatedRotation(q, fAnimTime, pNodeAnim);
		XMMATRIX RotationM = XMMatrixRotationQuaternion(XMVectorSet(q.x, q.y, q.z, q.w));


		aiVector3D t;
		CalcInterpolatedPosition(t, fAnimTime, pNodeAnim);
		XMMATRIX TranslationM = XMMatrixTranslation(t.x, t.y, t.z);


		NodeTransformation = ScalingM * RotationM * TranslationM; //������ * ȸ�� * �̵� ��ȯ
		NodeTransformation = XMMatrixTranspose(NodeTransformation);
		//transpos �Ƚ����ָ� �� ����
	}
	//�θ��忡 ��ȯ�� ��ø�ؼ� ���ϱ�
	XMMATRIX GlobalTransformation = ParentTransform * NodeTransformation;


	//�����尡 �� ����̸� ��ȯ������ ���� ����
	for (auto& p : m_Bones) {
		if (p.first == pNode->mName.data) {
			p.second.FinalTransformation =
				m_GlobalInverse * GlobalTransformation * p.second.BoneOffset;

			/*if (p.first == "Bip001 L Hand") {
				XMVECTOR tmp = p.second.BoneOffset.r[3];
				cout << "Left Hand BondeOffset: " << tmp.m128_f32[0] << "," << tmp.m128_f32[1] << "," << tmp.m128_f32[2] << endl;

				tmp = p.second.FinalTransformation.r[3];
				cout << "Left Hand FinalTransformation: " << tmp.m128_f32[0] << "," << tmp.m128_f32[1] << "," << tmp.m128_f32[2] << endl;
			}*/

			break;
		}
	}

	for (UINT i = 0; i < pNode->mNumChildren; ++i) {
		//���������� �̷�. �ڽĳ�� Ž�� �� ��ȯ
		ReadNodeHeirarchy(fAnimTime, pNode->mChildren[i], GlobalTransformation);
	}
}

const aiNodeAnim * LoadAnimation::FindNodeAnim(const aiAnimation * pAnimation, const string & NodeName)
{
	for (UINT i = 0; i < pAnimation->mNumChannels; ++i) {
		const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

		if (pNodeAnim->mNodeName.data == NodeName)
			return pNodeAnim;
	}
	return nullptr;
}

void LoadAnimation::CalcInterpolatedScaling(aiVector3D & Out, float AnimationTime, const aiNodeAnim * pNodeAnim)
{
	if (pNodeAnim->mNumScalingKeys == 1) {
		Out = pNodeAnim->mScalingKeys[0].mValue;
		return;
	}

	UINT ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
	UINT NextScalingIndex = ScalingIndex + 1;

	assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);

	float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;

	const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
	const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}

void LoadAnimation::CalcInterpolatedRotation(aiQuaternion & Out, float AnimationTime, const aiNodeAnim * pNodeAnim)
{
	if (pNodeAnim->mNumRotationKeys == 1) {
		Out = pNodeAnim->mRotationKeys[0].mValue;
		return;
	}

	UINT RotationIndex = FindRotation(AnimationTime, pNodeAnim);
	UINT NextRotationIndex = (RotationIndex + 1);

	assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);

	float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;

	const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
	aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
	Out = Out.Normalize();
}

void LoadAnimation::CalcInterpolatedPosition(aiVector3D & Out, float AnimationTime, const aiNodeAnim * pNodeAnim)
{
	if (pNodeAnim->mNumPositionKeys == 1) {
		Out = pNodeAnim->mPositionKeys[0].mValue;
		return;
	}

	UINT PositionIndex = FindPosition(AnimationTime, pNodeAnim);
	UINT NextPositionIndex = (PositionIndex + 1);


	assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);


	float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;

	const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
	const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}

unsigned int LoadAnimation::FindScaling(float AnimationTime, const aiNodeAnim * pNodeAnim)
{
	assert(pNodeAnim->mNumScalingKeys > 0);

	for (UINT i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
			return i;
		}
	}
	return 0;
}

unsigned int LoadAnimation::FindRotation(float AnimationTime, const aiNodeAnim * pNodeAnim)
{
	assert(pNodeAnim->mNumRotationKeys > 0);

	for (UINT i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
			return i;
		}
	}
	return 0;
}

unsigned int LoadAnimation::FindPosition(float AnimationTime, const aiNodeAnim * pNodeAnim)
{
	for (UINT i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
			return i;
		}
	}
	return 0;
}

void LoadAnimation::SetAnimFrame(float frame)
{
	if (frame >= end_time)
		frame = end_time;
	now_time = frame;
}

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
