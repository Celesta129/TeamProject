#include "stdafx.h"
#include "LoadModel.h"


ModelMesh::ModelMesh(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, mesh & meshData)
	
{
	m_nVertices = (int)meshData.m_vertices.size();
	m_nIndices = (int)meshData.m_indices.size();

	//cout << "���� ���� : " << m_nVertices << endl;

	m_nStride = sizeof(VertexData);
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_xmOOBB = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(10.f, 10.f, 10.f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

	//���ؽ� ����
	m_pd3dVertexBuffer = d3dUtil::CreateDefaultBuffer(pd3dDevice, pd3dCommandList, meshData.m_vertices.data(), m_nStride * m_nVertices, m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	//�ε��� ����
	m_pd3dIndexBuffer = d3dUtil::CreateDefaultBuffer(pd3dDevice, pd3dCommandList, meshData.m_indices.data(), sizeof(int)*m_nIndices, m_pd3dIndexUploadBuffer);

	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT)*m_nIndices;
}


LoadModel::LoadModel(const string & filename, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT flag = aiProcess_JoinIdenticalVertices |         // ������ ������ ����, �ε��� ����ȭ
		aiProcess_ValidateDataStructure |               // �δ��� ����� ����
		aiProcess_ImproveCacheLocality |               // ��� ������ ĳ����ġ�� ����
		aiProcess_RemoveRedundantMaterials |            // �ߺ��� ���͸��� ����
		aiProcess_GenUVCoords |                        // ����, ������, ���� �� ��� ������ ������ UV�� ��ȯ
		aiProcess_TransformUVCoords |                  // UV ��ȯ ó���� (�����ϸ�, ��ȯ...)
		aiProcess_FindInstances |                     // �ν��Ͻ��� �Ž��� �˻��Ͽ� �ϳ��� �����Ϳ� ���� ������ ����
		aiProcess_LimitBoneWeights |                  // ������ ���� ����ġ�� �ִ� 4���� ����
		aiProcess_OptimizeMeshes |                     // ������ ��� ���� �Ž��� ����
		aiProcess_GenSmoothNormals |                  // �ε巯�� �븻����(��������) ����
		aiProcess_SplitLargeMeshes |                  // �Ŵ��� �ϳ��� �Ž��� �����Ž���� ��Ȱ(����)
		aiProcess_Triangulate |                        // 3�� �̻��� �𼭸��� ���� �ٰ��� ���� �ﰢ������ ����(����)
		aiProcess_ConvertToLeftHanded |                  // D3D�� �޼���ǥ��� ��ȯ
		aiProcess_SortByPType;                        // ����Ÿ���� ������Ƽ��� ������ '������' �Ž��� ����
	
	m_pScene = aiImportFile(filename.c_str(), flag);

	if (m_pScene) {
		m_Meshes.resize(m_pScene->mNumMeshes);
		m_numBones = 0;
		InitScene();

		m_ModelMeshes.resize(m_Meshes.size());

		SetMeshes(pd3dDevice, pd3dCommandList);
		//
		//m_vpBonesMatrix.resize(m_Bones.size());		// �� ��ȯ���� �ʱ�ȭ
		//for (auto& p : m_vpBonesMatrix) {

		//	XMStoreFloat4x4(&*p, XMMatrixIdentity());
		//}

		InitAnimation();
	}
}

LoadModel::LoadModel(const LoadModel & T)
{
	m_pScene = T.m_pScene;

	m_Meshes = T.m_Meshes;
	m_ModelMeshes = T.m_ModelMeshes;
	m_Bones = T.m_Bones;

	m_numVertices = T.m_numVertices;
	m_numBones = T.m_numBones;

	m_vAnimation = T.m_vAnimation;
	m_vBonesMatrix = T.m_vBonesMatrix;

	m_posSize = T.m_posSize;
	m_numVertices = T.m_numVertices;
	m_numMaterial = T.m_numMaterial;
	m_numBones = T.m_numBones;
	m_GlobalInverse = T.m_GlobalInverse;
	
	m_bAnimationLoop = T.m_bAnimationLoop;
	m_fAnimSpeed = T.m_fAnimSpeed;
	m_fStart_time = T.m_fStart_time; //������ ���� �ð�
	m_fEnd_time = T.m_fEnd_time;  //������ ���� �ð�
	m_fTrigger_time = T.m_fTrigger_time;	//������ �߰� �ð�.Ʈ���ſ뵵�� ���
	m_fNow_time = T.m_fNow_time;  //���� ������
	m_fPosible_skip = T.m_fPosible_skip; //�ִϸ��̼��� ���� �����ϰ� ���� �ִϸ��̼� ���� ������ ������
}

LoadModel::~LoadModel()
{
}

void LoadModel::InitScene()
{
	for (UINT index = 0; index < m_Meshes.size(); ++index) {
		const aiMesh* pMesh = m_pScene->mMeshes[index];
		InitMesh(index, pMesh);

		if (pMesh->HasBones()) 
			InitBones(index, pMesh);

		m_numVertices += (UINT)m_Meshes[index].m_vertices.size();
	}
	m_numBones = (UINT)m_Bones.size();
	
}

void LoadModel::InitMesh(UINT index, const aiMesh * pMesh)
{
	m_Meshes[index].m_vertices.reserve(pMesh->mNumVertices);
	m_Meshes[index].m_indices.reserve(pMesh->mNumFaces * 3);

	m_posSize = pMesh->mNumVertices;
	for (UINT i = 0; i < pMesh->mNumVertices; ++i) {
		XMFLOAT3 zero_3(0.0f, 0.0f, 0.0f);
		XMFLOAT3 pos(0.0f, 0.0f, 0.0f);
		pos.x = pMesh->mVertices[i].x;
		pos.y = pMesh->mVertices[i].y;
		pos.z = pMesh->mVertices[i].z;

		XMFLOAT3 normal(0.0f, 0.0f, 0.0f);
		normal.x = pMesh->mNormals[i].x;
		normal.y = pMesh->mNormals[i].y;
		normal.z = pMesh->mNormals[i].z;

		XMFLOAT2 tex(0.0f, 0.0f);
		if (pMesh->HasTextureCoords(0)) {
			tex.x = pMesh->mTextureCoords[0][i].x;
			tex.y = pMesh->mTextureCoords[0][i].y;
		}
		else
			tex = XMFLOAT2(0.0f, 0.0f);
		//tangent�� �ϴ� 0���� �ʱ�ȭ
		const VertexData data(pos, normal, zero_3, tex, index);
		m_Meshes[index].m_vertices.push_back(data);
	}

	for (UINT i = 0; i < pMesh->mNumFaces; ++i) {
		const aiFace& face = pMesh->mFaces[i];
		m_Meshes[index].m_indices.push_back(face.mIndices[0]);
		m_Meshes[index].m_indices.push_back(face.mIndices[1]);
		m_Meshes[index].m_indices.push_back(face.mIndices[2]);
	}
	CalculateTangentArray(pMesh->mNumVertices, m_Meshes[index].m_vertices, pMesh->mNumFaces, m_Meshes[index].m_indices);
}

void LoadModel::InitBones(UINT index, const aiMesh * pMesh)
{
	for (UINT i = 0; i < pMesh->mNumBones; ++i) {
		int BoneIndex = -1;
		const aiBone* pBone = pMesh->mBones[i];

		int tmpIndex = 0;
		for (const auto& p : m_Bones) { //�̹� �����ϴ� ������ �˻�
			if (p.first == pBone->mName.data) {
				BoneIndex = tmpIndex;
				//���� ���� �̹� ���Ϳ� ����� ���� ���
				//�ε����� �ش� ���� �ε����� ����
				break;
			}
			tmpIndex++;
		}

		if (BoneIndex < 0) { //������ ���� �߰�
			BoneIndex = (int)m_Bones.size();
			//���� �����ϴ� ���� ��� 
			//�ε����� ���� ���� ���� (0���� ��� 0���� ����)

			Bone bone;
			bone.BoneOffset = aiMatrixToXMMatrix(pBone->mOffsetMatrix);
			m_Bones.emplace_back(make_pair(pBone->mName.data, bone));

			XMFLOAT4X4 bornMatrix;
			XMStoreFloat4x4(&bornMatrix, bone.FinalTransformation);
			m_vBonesMatrix.push_back(bornMatrix);
		}


		for (UINT b = 0; b < pBone->mNumWeights; ++b) {
			UINT vertexID = pBone->mWeights[b].mVertexId;
			float weight = pBone->mWeights[b].mWeight;
			m_Meshes[index].m_vertices[vertexID].AddBoneData(BoneIndex, weight);

		}
	}
}

void LoadModel::InitAnimation(void)
{
	if (m_pScene) {
		m_GlobalInverse = XMMatrixIdentity();
		for (UINT index = 0; index < m_pScene->mNumAnimations; ++index)
		{
			aiAnimation* pAnimation = *(m_pScene->mAnimations + index);
			m_vAnimation.push_back(pAnimation);
			m_fStart_time = 0.f;
			//������ ���� ������ ��ǥ �̵� �������� �������� ����
			m_fEnd_time = pAnimation->mDuration;
			
			//������ ���� �������� 1.0 ��ŭ ����� �������� �Ȱ�ħ
			m_fAnimSpeed = pAnimation->mTicksPerSecond;
		}
	}
}

void LoadModel::SetMeshes(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
	for (UINT i = 0; i < m_ModelMeshes.size(); ++i) {
		
		if (m_ModelMeshes[i] != nullptr)
			m_ModelMeshes[i].reset();	// ���� �񽺹����Ѱ���. ��·�� �����ڵ�
		
		m_ModelMeshes[i] = make_shared<ModelMesh>(pd3dDevice, pd3dCommandList, m_Meshes[i]);
	}
}

void LoadModel::SetAnimTime(const float & fTime)
{
	if (m_fEnd_time <= fTime)
		m_fNow_time = m_fEnd_time;
	else
		m_fNow_time = fTime;
}

UINT LoadModel::BornTransform(const UINT & Animindex, const float fTimeElapsed)
{
	
	XMMATRIX Identity = XMMatrixIdentity();
	if (!m_pScene) {
		//�ִϸ��̼� ������ �ε�� ���� ��� ����
		for (UINT i = 0; i < m_numBones; ++i) {
			XMStoreFloat4x4(&m_vBonesMatrix[i], Identity);
		}
		return LOOP_IN;
	}
	ReadNodeHeirarchy(Animindex, m_fNow_time, m_pScene->mRootNode, Identity);

	for (UINT bornIndex = 0; bornIndex < m_numBones; ++bornIndex)
	{
		XMStoreFloat4x4(&m_vBonesMatrix[bornIndex], m_Bones[bornIndex].second.FinalTransformation);
	}

	m_fNow_time += m_fAnimSpeed * fTimeElapsed;
	if (m_fNow_time > m_fEnd_time) {
		m_fNow_time = m_fStart_time;
		
		return LOOP_END; //�ִϸ��̼��� �� ���� ����
	}

	if (m_fNow_time > m_fTrigger_time - 1 && m_fNow_time < m_fTrigger_time + 1) {
		// ���� �ð��� Ʈ������ 1�� ���ܶ��

		return LOOP_TRIGGER;
	}

	if (m_fNow_time > m_fPosible_skip)
		return LOOP_SKIP; //�ִϸ��̼� ���� �������̰� Ʈ���� ���� ���Ĺݺ�

	return LOOP_IN; //�ִϸ��̼��� ���� ������
}

void LoadModel::ReadNodeHeirarchy(const UINT& Animindex, float AnimationTime, const aiNode * pNode, const XMMATRIX & ParentTransform)
{
	const aiAnimation* pAnim = m_vAnimation[Animindex];

	XMMATRIX NodeTransformation = aiMatrixToXMMatrix(pNode->mTransformation);

	const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnim, pNode->mName.data);
	
	if (pNodeAnim) {
		aiVector3D s;
		CalcInterpolatedScaling(s, AnimationTime, pNodeAnim);
		XMMATRIX ScalingM = XMMatrixScaling(s.x, s.y, s.z);


		aiQuaternion q;
		CalcInterpolatedRotation(q, AnimationTime, pNodeAnim);
		XMMATRIX RotationM = XMMatrixRotationQuaternion(XMVectorSet(q.x, q.y, q.z, q.w));


		aiVector3D t;
		CalcInterpolatedPosition(t, AnimationTime, pNodeAnim);
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
		ReadNodeHeirarchy(Animindex, AnimationTime, pNode->mChildren[i], GlobalTransformation);
	}
	
}

const aiNodeAnim * LoadModel::FindNodeAnim(const aiAnimation * pAnimation, const string & NodeName)
{
	for (UINT i = 0; i < pAnimation->mNumChannels; ++i) {
		const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

		if (pNodeAnim->mNodeName.data == NodeName)
			return pNodeAnim;
	}
	return nullptr;
}

void LoadModel::CalcInterpolatedScaling(aiVector3D & Out, float AnimationTime, const aiNodeAnim * pNodeAnim)
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

void LoadModel::CalcInterpolatedRotation(aiQuaternion & Out, float AnimationTime, const aiNodeAnim * pNodeAnim)
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

void LoadModel::CalcInterpolatedPosition(aiVector3D & Out, float AnimationTime, const aiNodeAnim * pNodeAnim)
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

UINT LoadModel::FindScaling(float AnimationTime, const aiNodeAnim * pNodeAnim)
{
	assert(pNodeAnim->mNumScalingKeys > 0);

	for (UINT i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
			return i;
		}
	}
	return 0;
}

UINT LoadModel::FindRotation(float AnimationTime, const aiNodeAnim * pNodeAnim)
{
	assert(pNodeAnim->mNumRotationKeys > 0);

	for (UINT i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
			return i;
		}
	}
	return 0;
}

UINT LoadModel::FindPosition(float AnimationTime, const aiNodeAnim * pNodeAnim)
{
	for (UINT i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
			return i;
		}
	}
	return 0;
}

CComponent * LoadModel::Clone()
{
	// Clone�ҽ� �����ؼ� ���� �Ѱ��ش�.
	LoadModel* pComponent = new LoadModel(*this);
	
	return pComponent;
}

int LoadModel::Free(void)
{
	if (m_Meshes.size() > 0) {
		m_Meshes.clear();
		//	cout << "LoadModel m_meshes.clear()" << endl;
	}
	if (m_pScene) {
		delete m_pScene;
	}

	return 0;
}
