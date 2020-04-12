#pragma once
#include "stdafx.h"

#include "Mesh.h"


class ModelMesh : public MMesh
{
public:
	ModelMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, mesh& meshData);
	virtual ~ModelMesh() {
	}
};

class LoadModel
{
private:
	const aiScene* m_pScene;	//모델
	vector<mesh> m_Meshes;		// 메쉬
	vector<shared_ptr<ModelMesh>> m_ModelMeshes;	//메쉬 정보 리소스
	vector<pair<string, aiBone>> m_Bones;		// 뼈


	UINT                     m_posSize;
	UINT m_numVertices;
	UINT m_numMaterial;
	UINT                     m_numBones;

public:
	LoadModel(const string& filename);
	LoadModel(const LoadModel& T);
	~LoadModel();

	void InitScene();
	void InitMesh(UINT index, const aiMesh* pMesh);
	void InitBones(UINT index, const aiMesh* pMesh);

	void SetMeshs(ID3D12Device* pd3dDevice, ID3D12CommandList* pd3dCommandList);
	void SetTextureIndex(UINT meshIndex, UINT textureIndex) { m_Meshes[meshIndex].SetMeshesTextureIndex(textureIndex); }
	



	//ModelMesh** GetMeshs() { return m_ModelMeshes.Data(); }		// ????
	UINT GetNumMesh() const { return (UINT)m_Meshes.size(); }
};
