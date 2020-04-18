#include "stdafx.h"
#include "LoadModel.h"


ModelMesh::ModelMesh(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, mesh & meshData)
	:MMesh(pd3dDevice, pd3dCommandList)
{
}


LoadModel::LoadModel(const string & filename)
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
	}
}

LoadModel::LoadModel(const LoadModel & T)
{
}

LoadModel::~LoadModel()
{
}

void LoadModel::InitScene()
{
	for (UINT i = 0; i < m_Meshes.size(); ++i) {
		const aiMesh* pMesh = m_pScene->mMeshes[i];
		InitMesh(i, pMesh);

		if (pMesh->HasBones()) InitBones(i, pMesh);

		m_numVertices += (UINT)m_Meshes[i].m_vertices.size();
	}
	//m_numBones = (UINT)m_Bones.size();
	
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
	//for (UINT i = 0; i < pMesh->mNumBones; ++i) {
	//	int BoneIndex = -1;
	//	const aiBone* pBone = pMesh->mBones[i];

	//	int tmpIndex = 0;
	//	for (const auto& p : m_Bones) { //�̹� �����ϴ� ������ �˻�
	//		if (p.first == pBone->mName.data) {
	//			BoneIndex = tmpIndex;
	//			//���� ���� �̹� ���Ϳ� ����� ���� ���
	//			//�ε����� �ش� ���� �ε����� ����
	//			break;
	//		}
	//		tmpIndex++;
	//	}

	//	if (BoneIndex < 0) { //������ ���� �߰�
	//		BoneIndex = (int)m_Bones.size();
	//		//���� �����ϴ� ���� ��� 
	//		//�ε����� ���� ���� ���� (0���� ��� 0���� ����)

	//		Bone bone;
	//		bone.BoneOffset = aiMatrixToXMMatrix(pBone->mOffsetMatrix);
	//		m_Bones.emplace_back(make_pair(pBone->mName.data, bone));
	//	}


	//	for (UINT b = 0; b < pBone->mNumWeights; ++b) {
	//		UINT vertexID = pBone->mWeights[b].mVertexId;
	//		float weight = pBone->mWeights[b].mWeight;
	//		m_Meshes[index].m_vertices[vertexID].AddBoneData(BoneIndex, weight);

	//	}
	//}
}

void LoadModel::SetMeshs(ID3D12Device * pd3dDevice, ID3D12CommandList * pd3dCommandList)
{
}