#pragma once
#include "stdafx.h"

#include "LoadModel.h"
#include "LoadAnimation.h"
#include "Model_TextureObject.h"

class Model_Animation : public CComponent
{
public:
	Model_Animation();
	Model_Animation(const string Model_filename, ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, const vector<pair<string, float>>& Animation_filename);
	virtual ~Model_Animation();
public:
	void LoadingModels(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	LoadModel* GetModel(void) { return m_pModel; }
	UINT GetAnimCount(void) { return (UINT)m_vAnimStack->size(); }
	LoadAnimation** GetAnim(UINT index) {
		return m_vAnimStack[index].data();
	}
	wstring getMat(UINT index) {
		return wstring(normalMatList[index].begin(), normalMatList[index].end());
	}
	bool isMat(UINT index) {
		if (matList[index] != "null")
			return true;
		return false;
	}
	bool isNormat(UINT index) {
		if (normalMatList[index] != "null")
			return true;
		return false;
	}
protected:
	LoadModel* m_pModel;
	vector<LoadAnimation*>* m_vAnimStack;
	vector<string> matList;
	vector<string> normalMatList;


public:
	virtual CComponent* Clone(void);
};
