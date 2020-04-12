#pragma once
#include "stdafx.h"

#include "Mesh.h"

#include "assimp/Importer.hpp"   // assimp ¸ðµ¨ import
#include "assimp/cimport.h"
#include "assimp/postprocess.h"
#include "assimp/scene.h"


#pragma comment(lib, "assimp-vc140-mt.lib")
class ModelMesh : public CMesh
{
public:
	ModelMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, mesh& meshData);
	virtual ~ModelMesh() {
	}
};

class LoadModel
{
private:
	const aiScene* m_pScene;	//¸ðµ¨
	vector<CMesh> m_Meshes;		// ¸Þ½¬
	vector<shared_ptr<ModelMesh>> m_ModelMeshes;	//¸Þ½¬ Á¤º¸ ¸®¼Ò½º
	vector<pair<string, aiBone>> m_Bones;		// »À


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
