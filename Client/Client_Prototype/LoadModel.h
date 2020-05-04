#pragma once
#include "stdafx.h"
#include "Mesh.h"
#include "Component.h"
#include "Animation.h"

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
	const aiScene* m_pScene;	//모델
	vector<CAnimation> m_vAnimation;

	vector<mesh> m_Meshes;		// 메쉬
	vector<shared_ptr<ModelMesh>> m_ModelMeshes;	//메쉬 정보 리소스
	vector<pair<string, Bone>> m_Bones;		// 뼈
	//vector<vector<XMFLOAT4X4>> m_vvBonesMatrix;					

	vector<XMFLOAT4X4> m_vBonesMatrix; // 뼈의 최종변환 매트릭스.

	// vector<ANIMATION> : aiAnimation*, m_vBonesMatrix,
	UINT                    m_posSize;
	UINT					m_numVertices;
	UINT					m_numMaterial;
	UINT                    m_numBones;
	XMFLOAT4X4				m_GlobalInverse;

	UINT					m_currAnimIndex = 0;		// 사실상 오브젝트에서는 모델 여러개를 사용하는것이지만 일단은 파일 임포트 과정에서 여러개 부를수도 있으니 만들어둔다.
public:
	LoadModel(const string& filename, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	LoadModel(const LoadModel& T);
	~LoadModel();

	void InitScene();
	void InitMesh(UINT index, const aiMesh* pMesh);
	void InitBones(UINT index, const aiMesh* pMesh);
	void InitAnimation(const aiScene* pScene);

	void SetMeshes(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void SetTextureIndex(UINT meshIndex, UINT textureIndex) { m_Meshes[meshIndex].SetMeshesTextureIndex(textureIndex); }
	void SetAnimTime(const float& fTime);
	void SetCurrAnimIndex(UINT AnimIndex);

	UINT BornTransform(const float fTimeElapsed);	// 애니메이션 코드
	
	void Render(ID3D12GraphicsCommandList * pCommandList);
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
	UINT getCurrAnimIndex(void) {
		return m_currAnimIndex;
	}
	UINT getNumAnimations(void) {
		return m_vAnimation.size();
	}
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
