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
	const aiScene* m_pScene;	//모델
	vector<mesh> m_Meshes;		// 메쉬
	vector<shared_ptr<ModelMesh>> m_ModelMeshes;	//메쉬 정보 리소스
	vector<pair<string, Bone>> m_Bones;		// 뼈


	UINT                     m_posSize;
	UINT					m_numVertices;
	UINT					m_numMaterial;
	UINT                     m_numBones;

public:
	LoadModel(const string& filename, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	LoadModel(const LoadModel& T);
	~LoadModel();

	void InitScene();
	void InitMesh(UINT index, const aiMesh* pMesh);
	void InitBones(UINT index, const aiMesh* pMesh);

	void SetMeshes(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void SetTextureIndex(UINT meshIndex, UINT textureIndex) { m_Meshes[meshIndex].SetMeshesTextureIndex(textureIndex); }
	

	shared_ptr<ModelMesh>*          getMeshes() { return m_ModelMeshes.data(); }
	mesh*                  getMesh(UINT index) { return &m_Meshes[index]; }
	UINT                  getNumMesh() const { return (UINT)m_Meshes.size(); }
	vector<pair<string, Bone>>* GetBones() { return &m_Bones; }
	UINT                  getNumVertices() const { return m_numVertices; }


public:
	virtual CComponent* Clone();
	virtual int Free(void);
};
