#include "stdafx.h"
#include "LoadModel.h"


ModelMesh::ModelMesh(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, mesh & meshData)

{
	m_nVertices = (int)meshData.m_vertices.size();
	m_nIndices = (int)meshData.m_indices.size();

	//cout << "정점 개수 : " << m_nVertices << endl;

	m_nStride = sizeof(VertexData);
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_xmOOBB = BoundingOrientedBox(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(10.f, 10.f, 10.f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));

	//버텍스 버퍼
	m_pd3dVertexBuffer = d3dUtil::CreateDefaultBuffer(pd3dDevice, pd3dCommandList, meshData.m_vertices.data(), m_nStride * m_nVertices, m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	//인덱스 버퍼
	m_pd3dIndexBuffer = d3dUtil::CreateDefaultBuffer(pd3dDevice, pd3dCommandList, meshData.m_indices.data(), sizeof(int)*m_nIndices, m_pd3dIndexUploadBuffer);

	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT)*m_nIndices;
}


LoadModel::LoadModel(const string & filename, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	UINT flag = aiProcess_JoinIdenticalVertices |         // 동일한 꼭지점 결합, 인덱싱 최적화
		aiProcess_ValidateDataStructure |               // 로더의 출력을 검증
		aiProcess_ImproveCacheLocality |               // 출력 정점의 캐쉬위치를 개선
		aiProcess_RemoveRedundantMaterials |            // 중복된 매터리얼 제거
		aiProcess_GenUVCoords |                        // 구형, 원통형, 상자 및 평면 매핑을 적절한 UV로 변환
		aiProcess_TransformUVCoords |                  // UV 변환 처리기 (스케일링, 변환...)
		aiProcess_FindInstances |                     // 인스턴스된 매쉬를 검색하여 하나의 마스터에 대한 참조로 제거
		aiProcess_LimitBoneWeights |                  // 정점당 뼈의 가중치를 최대 4개로 제한
		aiProcess_OptimizeMeshes |                     // 가능한 경우 작은 매쉬를 조인
		aiProcess_GenSmoothNormals |                  // 부드러운 노말벡터(법선벡터) 생성
		aiProcess_SplitLargeMeshes |                  // 거대한 하나의 매쉬를 하위매쉬들로 분활(나눔)
		aiProcess_Triangulate |                        // 3개 이상의 모서리를 가진 다각형 면을 삼각형으로 만듬(나눔)
		aiProcess_ConvertToLeftHanded |                  // D3D의 왼손좌표계로 변환
		aiProcess_SortByPType;                        // 단일타입의 프리미티브로 구성된 '깨끗한' 매쉬를 만듬

	m_pScene = aiImportFile(filename.c_str(), flag);

	if (m_pScene) {
		m_Meshes.resize(m_pScene->mNumMeshes);
		m_numBones = 0;
		InitScene();

		m_ModelMeshes.resize(m_Meshes.size());

		SetMeshes(pd3dDevice, pd3dCommandList);
		
		m_vBonesMatrix.resize(m_Bones.size());		// 뼈 변환정보 초기화
		for (auto& p : m_vBonesMatrix) {

			XMStoreFloat4x4(&p, XMMatrixIdentity());
		}

		InitAnimation(m_pScene);
	}
}

LoadModel::LoadModel(const LoadModel & T)
{
	m_pScene = T.m_pScene;

	m_Meshes = T.m_Meshes;
	m_ModelMeshes = T.m_ModelMeshes;
	m_Bones = T.m_Bones;

	m_vBonesMatrix = T.m_vBonesMatrix;

	m_numVertices = T.m_numVertices;
	m_numBones = T.m_numBones;

	m_vAnimation = T.m_vAnimation;

	m_posSize = T.m_posSize;
	m_numVertices = T.m_numVertices;
	m_numMaterial = T.m_numMaterial;
	m_numBones = T.m_numBones;
	m_GlobalInverse = T.m_GlobalInverse;
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

		for (int j = 0; j < pMesh->mNumBones; ++j)
		{
			const aiBone* pBone = pMesh->mBones[j];
			Bone bone;
			bone.BoneOffset = aiMatrixToXMMatrix(pBone->mOffsetMatrix);

			XMFLOAT4X4 bornMatrix;
			XMStoreFloat4x4(&bornMatrix, bone.FinalTransformation);
			m_vBonesMatrix.push_back(bornMatrix);

		}
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
		//tangent는 일단 0으로 초기화
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
		for (const auto& p : m_Bones) { //이미 존재하는 뼈인지 검색
			if (p.first == pBone->mName.data) {
				BoneIndex = tmpIndex;
				//현재 뼈가 이미 벡터에 저장된 뼈일 경우
				//인덱스를 해당 뼈의 인덱스로 저장
				break;
			}
			tmpIndex++;
		}

		if (BoneIndex < 0) { //없으면 새로 추가
			BoneIndex = (int)m_Bones.size();
			//새로 저장하는 뼈일 경우 
			//인덱스는 현재 뼈의 개수 (0개일 경우 0부터 시작)

			Bone bone;
			bone.BoneOffset = aiMatrixToXMMatrix(pBone->mOffsetMatrix);
			m_Bones.emplace_back(make_pair(pBone->mName.data, bone));

			
		}


		for (UINT b = 0; b < pBone->mNumWeights; ++b) {
			UINT vertexID = pBone->mWeights[b].mVertexId;
			float weight = pBone->mWeights[b].mWeight;
			m_Meshes[index].m_vertices[vertexID].AddBoneData(BoneIndex, weight);

		}
	}
}

void LoadModel::InitAnimation(const aiScene* pScene)
{
	if (pScene) {
		m_GlobalInverse = Matrix4x4::Identity();
		for (UINT index = 0; index < pScene->mNumAnimations; ++index)
		{
			aiAnimation* paiAnimation = *(pScene->mAnimations + index);
			CAnimation Anim;

			Anim.SetaiAnim(paiAnimation);

			//
			
			//
			Anim.m_fEnd_time = paiAnimation->mDuration;
			Anim.m_fAnimSpeed = paiAnimation->mTicksPerSecond;
			m_vAnimation.push_back(Anim);
		}
	}
}

void LoadModel::SetMeshes(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
	for (UINT i = 0; i < m_ModelMeshes.size(); ++i) {

		if (m_ModelMeshes[i] != nullptr)
			m_ModelMeshes[i].reset();	// 삭제 비스무리한거임. 어쨌든 삭제코드

		m_ModelMeshes[i] = make_shared<ModelMesh>(pd3dDevice, pd3dCommandList, m_Meshes[i]);
	}
}

void LoadModel::SetAnimTime(const float & fTime)
{
	CAnimation* pAnimation = &m_vAnimation[m_currAnimIndex];
	if (pAnimation->m_fEnd_time <= fTime)
		pAnimation->m_fNow_time = pAnimation->m_fEnd_time;
	else
		pAnimation->m_fNow_time = fTime;
}

void LoadModel::SetCurrAnimIndex(UINT AnimIndex)
{
	if (m_vAnimation.size() > AnimIndex)
	{
		m_currAnimIndex = AnimIndex;
		m_vAnimation[m_currAnimIndex].m_fNow_time = 0.0f;
		SetAnimTime(0.0f);
	}
	
}

UINT LoadModel::BornTransform(const float fTimeElapsed)
{

	XMMATRIX Identity = XMMatrixIdentity();
	CAnimation* pAnimation = &m_vAnimation[m_currAnimIndex];

	if (!m_pScene) {
		//애니메이션 파일을 로드못 했을 경우 수행
		for (UINT i = 0; i < m_numBones; ++i) {

			XMStoreFloat4x4(&m_vBonesMatrix[i], Identity);
		}
		return LOOP_IN;
	}
	ReadNodeHeirarchy(m_currAnimIndex, pAnimation->m_fNow_time, m_pScene->mRootNode, Identity);

	for (UINT bornIndex = 0; bornIndex < m_numBones; ++bornIndex)
	{
		XMStoreFloat4x4(&m_vBonesMatrix[bornIndex], m_Bones[bornIndex].second.FinalTransformation);
	}

	pAnimation->m_fNow_time += pAnimation->m_fAnimSpeed * fTimeElapsed;
	if (pAnimation->m_fNow_time > pAnimation->m_fEnd_time) {
		pAnimation->m_fNow_time = pAnimation->m_fStart_time;

		return LOOP_END; //애니메이션이 한 루프 끝남
	}

	if (pAnimation->m_fNow_time > pAnimation->m_fTrigger_time - 1 && pAnimation->m_fNow_time < pAnimation->m_fTrigger_time + 1) {
		// 현재 시간이 트리거의 1초 내외라면

		return LOOP_TRIGGER;
	}

	if (pAnimation->m_fNow_time > pAnimation->m_fPosible_skip)
		return LOOP_SKIP; //애니메이션 아직 실행중이고 트리거 실행 후후반부

	return LOOP_IN; //애니메이션이 아직 실행중
}

void LoadModel::Render(ID3D12GraphicsCommandList * pCommandList)
{
	for (UINT i = 0; i < m_ModelMeshes.size(); ++i)
	{
		m_ModelMeshes[i]->Render(pCommandList);
	}
}


void LoadModel::ReadNodeHeirarchy(const UINT& Animindex, float AnimationTime, const aiNode * pNode, const XMMATRIX & ParentTransform)
{
	const aiAnimation* pAnim = m_vAnimation[Animindex].GetAnim();

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


		NodeTransformation = ScalingM * RotationM * TranslationM; //스케일 * 회전 * 이동 변환
		NodeTransformation = XMMatrixTranspose(NodeTransformation);
		//transpos 안시켜주면 모델 깨짐
	}
	//부모노드에 변환값 중첩해서 곱하기
	XMMATRIX GlobalTransformation = ParentTransform * NodeTransformation;
	//현재노드가 뼈 노드이면 변환정보를 뼈에 적용
	XMMATRIX GlobalInverse = XMLoadFloat4x4(&m_GlobalInverse);
	for (auto& p : m_Bones) {
		if (p.first == pNode->mName.data) {
			p.second.FinalTransformation =
				GlobalInverse * GlobalTransformation * p.second.BoneOffset;

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
		//계층구조를 이룸. 자식노드 탐색 및 변환
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
	// Clone할시 복사해서 새로 넘겨준다.
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
		Safe_Delete(m_pScene);
	}

	return 0;
}
