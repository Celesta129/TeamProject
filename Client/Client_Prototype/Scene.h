#pragma once
#include "../Common/UploadBuffer.h"
#include "Component_Manager.h"
#include "FrameResource.h"
#include "GameObject.h"

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

	//씬에서 마우스와 키보드 메시지를 처리한다. 
	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseObjects();

	virtual void AnimateObjects(float fTimeElapsed);

	virtual void Update(float fTimeElapsed, ID3D12Fence* pFence);
	virtual void Render(UINT frameResourceIndex, ID3D12GraphicsCommandList* cmdList, UINT CbvSrvUavDescriptorSize);

	void ReleaseUploadBuffers();

	void OnResize(float fAspectRatio);

public:
	ComPtr<ID3D12PipelineState> GetPipelineStates(void) {
		return m_PSO;
	}
	FrameResource* GetCurrentFrameResource(void) {
		return m_CurrentFrameResource;
	}
	int GetCurrentFrameResourceIndex(void) {
		return m_CurrentFrameResourceIndex;
	}

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
	static const int NumFrameResources = 3;
	vector<unique_ptr<FrameResource>> m_vFrameResources;
	FrameResource* m_CurrentFrameResource = nullptr;
	int m_CurrentFrameResourceIndex = 0;

protected:
	ComPtr<ID3D12Device> m_d3dDevice;
	ComPtr<ID3D12GraphicsCommandList> m_GraphicsCommandList;

	//배치(Batch) 처리를 하기 위하여 씬을 셰이더들의 리스트로 표현한다. 
	//CObjectShader *m_pShaders = NULL;
	//int m_nShaders = 0;

	ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
	ComPtr<ID3D12DescriptorHeap> mCbvHeap = nullptr;		// ConstantBufferViewHeap
	
	UINT mPassCbvOffset = 0;

	UINT m_CbvSrvUavDescriptorSize = 120;

	ComPtr<ID3DBlob> m_vsByteCode = nullptr;		// Vertex Shader
	ComPtr<ID3DBlob> m_psByteCode = nullptr;		// Pixel Shader

	vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;

	ComPtr<ID3D12PipelineState> m_PSO = nullptr;		// Pipeline State Object


protected:
	// List of all the render items.
	std::vector<std::unique_ptr<RenderItem>> mAllRitems;
	std::vector<std::unique_ptr<CGameObject>> m_vObjects;
	// Render items divided by PSO.
	std::vector<RenderItem*> mOpaqueRitems;
	std::vector<CGameObject*> m_OpaqueObjects;

protected:
	CCamera* m_pCamera = nullptr;
	CBoxMesh* m_pBoxMesh = nullptr;

	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> mGeometries;
	std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> mPSOs;
	std::unordered_map<std::string, ComPtr<ID3DBlob>> mShaders;
	vector<CShader*> m_vShaders;

protected:
	CComponent_Manager* m_pComponent_Manager = nullptr;


	DXGI_FORMAT m_BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT m_DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	bool m_b4xMsaaState = true;
	UINT m_n4xMsaaQuality = 0;

};


