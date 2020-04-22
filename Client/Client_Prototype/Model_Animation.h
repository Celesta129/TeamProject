#pragma once
#include "LoadModel.h"

class LoadAnimation
{
private:
	const aiScene* m_pScene;
	const aiAnimation* m_pAnim;
	vector<pair<string, Bone>> m_Bones;
	UINT m_NumBones;

	XMMATRIX m_GlobalInverse;	// 모델 글로벌 매트릭스
	//XMMATRIX m_grab;	
	//XMFLOAT4 m_handpos;

	float start_time;
	float end_time;
	float trigger_time;
	float now_time;		// 현재 애니메이션 시간
	float posible_skip;	// 현재 애니메이션을 스킵 가능한 시간

	BOOL animation_Loop = true;	//루프 여부
	BOOL stop_anim = false;// 중단가능한지
	UINT next_index = 0;

	float m_fAnimSpeed = 1.0f;
	float m_fPrevSpeed = 1.0f;
	float m_fInitSpeed = 1.0f;

public:
	LoadAnimation(string filename, float trigger, float skip);
	LoadAnimation(const LoadAnimation& T);
	~LoadAnimation();
public:
	void SetBones(vector<pair<string, Bone>>* bones) {
		m_Bones = *bones;
		m_NumBones = (UINT)m_Bones.size();
	}
	void DisableLoop(UINT index) {
		animation_Loop = false;
		next_index = index;
	}
	void EnableLoop(void) {
		animation_Loop = true;
	}
	void PlayAnim(bool swt) { stop_anim = swt; }

	void SetAnimSpeed(float speed) {
		m_fAnimSpeed = speed;
	}
	void LoadAnimSpeed() {
		m_fAnimSpeed = m_fPrevSpeed;
	}
	void SaveAnimSpeed() {
		m_fPrevSpeed = m_fAnimSpeed;
	}
	void InitSpeed() {
		m_fAnimSpeed = m_fInitSpeed;
	}


	UINT BoneTransform(UINT& index, float fTime, vector<XMFLOAT4X4>& transforms);
	void ReadNodeHeirarchy(float fAnimTime, const aiNode* pNode, const XMMATRIX& ParentTransform);
	const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const string& NodeName);

	void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);

	unsigned int FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
	unsigned int FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
	unsigned int FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);

	void ResetAnimation() { now_time = 0; }
	void SetAnimFrame(float frame);

	float getAnimTime() const { return now_time; }

	//XMFLOAT4 getHandPos() { return m_handpos; }


};
class Model_Animation : public CComponent
{
public:
	Model_Animation();
	Model_Animation(const string Model_filename, vector<pair<string, float>>* Animation_filename);
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

