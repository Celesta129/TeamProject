#pragma once
#include "stdafx.h"
#include "Base.h"
#include "Component_Manager.h"


class CTimer;
class CCamera;

class CGameObject;
class CShader;

class CScene : public CBase
{
public:
	CScene(ComPtr<ID3D12Device> pDevice, ComPtr<ID3D12GraphicsCommandList> pCommandList);
	virtual ~CScene();

	virtual HRESULT Initialize();
	//씬에서 마우스와 키보드 메시지를 처리한다. 
	virtual bool OnKeyboardInput(const float& fTimeElapsed);
	virtual void OnMouseDown(WPARAM btnState, int x, int y, const POINT& lastPoint);
	virtual void OnMouseMove(WPARAM btnState, int x, int y, const POINT& lastPoint);
	virtual void OnMouseUp(WPARAM btnState, int x, int y, const POINT& LastPoint);
	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	
	virtual void ResetCmdList(ID3D12GraphicsCommandList* pd3dCommandList);


	virtual void UpdateCamera(const float& fTimeElapsed);
	virtual void Update(const CTimer& timer, ID3D12Fence* pFence);
	virtual void AnimateObjects(float fTimeElapsed);
	virtual void Render(ID3D12GraphicsCommandList* cmdList, UINT64& nFenceValue);

	void ReleaseUploadBuffers();

	void OnResize(float fAspectRatio);

	// ------------for release-------------------
	virtual void ReleaseScene(void);
	virtual int Free(void);
protected:
	virtual void ReleaseShaders(void);
	virtual void ReleaseCameras(void);
	virtual void ReleaseObjects(void);
	//-------------------------------------------
protected:
	virtual void BuildComponents(void);
	virtual void BuildRootSignature(void);
	virtual void BuildShadersAndInputLayout(void);
	virtual void BuildPSOs(void);
	virtual void BuildFrameResources(void);
	virtual void BuildShapeGeometry(void);
	virtual void BuildRenderItems(void);

	virtual void BuildShaders();
	virtual void BuildMesh(void);
	virtual void BuildCamera(void);
	virtual void BuildObject(void);


protected:
	ComPtr<ID3D12Device> m_d3dDevice;
	ComPtr<ID3D12GraphicsCommandList> m_GraphicsCommandList;

protected:
	vector<CGameObject*> m_vObjects;

	// -----------for camera--------------------
	vector<CCamera*> m_vCameras;
	CCamera* m_pCurrentCamera = nullptr;
	
	//---------------------------------------------

	vector<CShader*> m_vShaders;

protected:
	CComponent_Manager* m_pComponent_Manager = nullptr;

};


