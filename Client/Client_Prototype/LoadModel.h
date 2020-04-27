#pragma once
#include "stdafx.h"
#include "Mesh.h"
#include "Component.h"

class ModelMesh : public CMesh
{
public:
	ModelMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, mesh& meshData);
	virtual ~ModelMesh() {
	}
};


class LoadModel : public CComponent
{
private:
	const aiScene* m_pScene;	//��
	vector<aiAnimation*> m_vAnimation;
	vector<mesh> m_Meshes;		// �޽�
	vector<shared_ptr<ModelMesh>> m_ModelMeshes;	//�޽� ���� ���ҽ�
	vector<pair<string, Bone>> m_Bones;		// ��
	vector<XMFLOAT4X4> m_vBonesMatrix;					// ���� ������ȯ ��Ʈ����.

	UINT                    m_posSize;
	UINT					m_numVertices;
	UINT					m_numMaterial;
	UINT                    m_numBones;
	XMMATRIX				m_GlobalInverse;
private:
	bool					m_bAnimationLoop = true;
	float					m_fAnimSpeed = 1.f;
	float					m_fStart_time = 0.f; //������ ���� �ð�
	float					m_fEnd_time = 0.f;  //������ ���� �ð�
	float					m_fTrigger_time = 0.f;	//������ �߰� �ð�.Ʈ���ſ뵵�� ���
	float					m_fNow_time = 0.f;  //���� ������
	float					m_fPosible_skip = 0.f; //�ִϸ��̼��� ���� �����ϰ� ���� �ִϸ��̼� ���� ������ ������
public:
	LoadModel(const string& filename, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	LoadModel(const LoadModel& T);
	~LoadModel();

	void InitScene();
	void InitMesh(UINT index, const aiMesh* pMesh);
	void InitBones(UINT index, const aiMesh* pMesh);
	void InitAnimation(void);

	void SetMeshes(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void SetTextureIndex(UINT meshIndex, UINT textureIndex) { m_Meshes[meshIndex].SetMeshesTextureIndex(textureIndex); }
	void SetAnimTime(const float& fTime);


	UINT BornTransform(const UINT& Animindex, const float fTimeElapsed);	// �ִϸ��̼� �ڵ�
public:
	bool HasAnimation(void) { return !m_vAnimation.empty(); }
	shared_ptr<ModelMesh>*          getMeshes() { return m_ModelMeshes.data(); }
	mesh*                  getMesh(UINT index) { return &m_Meshes[index]; }
	UINT                  getNumMesh() const { return (UINT)m_Meshes.size(); }
	vector<pair<string, Bone>>* GetBones() { return &m_Bones; }
	UINT                  getNumVertices() const { return m_numVertices; }

	UINT getNumBones(void) {
		return m_numBones;
	}
	const vector<XMFLOAT4X4>& getBonesTransform(void){
		return m_vBonesMatrix;
	};
private:
	void ReadNodeHeirarchy(const UINT& Animindex, float AnimationTime, const aiNode* pNode, const XMMATRIX& ParentTransform);
	const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const string& NodeName);

	void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	
	UINT FindScaling(float AnimationTime, const aiNodeAnim * pNodeAnim);
	UINT FindRotation(float AnimationTime, const aiNodeAnim * pNodeAnim);
	UINT FindPosition(float AnimationTime, const aiNodeAnim * pNodeAnim);

public:
	virtual CComponent* Clone();
	virtual int Free(void);
};
