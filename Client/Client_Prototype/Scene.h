#pragma once
#include "../Common/UploadBuffer.h"
#include "FrameResource.h"

class CGameObject;
class CCamera;
class CBox;
class CBoxMesh;

class CScene
{
public:
	CScene(ComPtr<ID3D12Device> pDevice, ComPtr<ID3D12GraphicsCommandList> pCommandList);
	virtual ~CScene();

	bool Initialize(UINT CbvSrvUavDescriptorSize);

	//씬에서 마우스와 키보드 메시지를 처리한다. 
	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	void ReleaseObjects();

	void AnimateObjects(float fTimeElapsed);

	virtual void Update(float fTimeElapsed, ID3D12Fence* pFence);
	virtual void Render(UINT CbvSrvUavDescriptorSize);

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
	void BuildRootSignature(void);
	void BuildDescriptorHeaps(void);
	void BuildConstantBuffersViews(UINT CbvSrvUavDescriptorSize);
	void BuildShadersAndInputLayout(void);
	void BuildPSO(void);
	void BuildFrameResources(void);
	void BuildMesh(void);
	void BuildObject(void);

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

	ComPtr<ID3D12RootSignature> m_d3dGraphicsRootSignature = nullptr;
	ComPtr<ID3D12DescriptorHeap> m_CbvHeap = nullptr;		// ConstantBufferViewHeap
	
	UINT m_PassCbvOffset = 0;

	ComPtr<ID3DBlob> m_vsByteCode = nullptr;		// Vertex Shader
	ComPtr<ID3DBlob> m_psByteCode = nullptr;		// Pixel Shader

	vector<D3D12_INPUT_ELEMENT_DESC> m_InputLayout;

	ComPtr<ID3D12PipelineState> m_PSO = nullptr;		// Pipeline State Object

	vector<unique_ptr<CObject>> m_vAllObjects;	// 모든 오브젝트
	vector<CObject*> m_vObjects;		// 같은 PSO를 사용하는 렌더링 오브젝트는 같은 목록에 둔다.
protected:
	CCamera* m_pCamera = nullptr;
	CBoxMesh* m_pBoxMesh = nullptr;
};

