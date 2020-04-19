#pragma once
#include "../Common/UploadBuffer.h"
#include "Component_Manager.h"
#include "FrameResource.h"
#include "GameObject.h"

class CTimer;
class CGameObject;
class CCamera;
class CBox;
class CBoxMesh;

class CShader;
class CScene
{
public:
	CScene(ComPtr<ID3D12Device> pDevice, ComPtr<ID3D12GraphicsCommandList> pCommandList);
	virtual ~CScene();

	virtual bool Initialize(UINT CbvSrvUavDescriptorSize);
	virtual void BuildShaders();
	//씬에서 마우스와 키보드 메시지를 처리한다. 
	virtual bool OnKeyboardInput(const float& fTimeElapsed);
	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseObjects();

	virtual void AnimateObjects(float fTimeElapsed);


	virtual void UpdateCamera(const float& fTimeElapsed);
	virtual void Update(const CTimer& timer, ID3D12Fence* pFence, ID3D12GraphicsCommandList * cmdList);
	virtual void Render(UINT frameResourceIndex, ID3D12GraphicsCommandList* cmdList, UINT CbvSrvUavDescriptorSize);

	void ReleaseUploadBuffers();

	void OnResize(float fAspectRatio);

protected:
	virtual void BuildComponents(void);
	virtual void BuildRootSignature(void);
	virtual void BuildDescriptorHeaps(void);
	virtual void BuildConstantBufferViews(UINT CbvSrvUavDescriptorSize);
	virtual void BuildShadersAndInputLayout(void);
	virtual void BuildPSOs(void);
	virtual void BuildFrameResources(void);
	virtual void BuildShapeGeometry(void);
	virtual void BuildRenderItems(void);

	virtual void BuildMesh(void);
	virtual void BuildObject(void);


protected:
	ComPtr<ID3D12Device> m_d3dDevice;
	ComPtr<ID3D12GraphicsCommandList> m_GraphicsCommandList;

	//배치(Batch) 처리를 하기 위하여 씬을 셰이더들의 리스트로 표현한다. 
	//CObjectShader *m_pShaders = NULL;
	//int m_nShaders = 0;

	ComPtr<ID3D12RootSignature> mRootSignature = nullptr;

protected:
	vector<CCamera*> m_vCameras;
	CCamera* m_pCurrentCamera = nullptr;
	CBoxMesh* m_pBoxMesh = nullptr;

	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> mGeometries;
	vector<CShader*> m_vShaders;

protected:
	CComponent_Manager* m_pComponent_Manager = nullptr;

};


