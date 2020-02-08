#pragma once
#include "stdafx.h"
#include "resource.h"
#include "D3DApp.h"
#include "RenderItem.h"
#include "FrameResource.h"

#include "Camera.h"
#include "GameObject.h"
//class CScene;

class CGameFramework_Client : public CD3DApp
{
public:
	CGameFramework_Client(HINSTANCE hInstance);
	CGameFramework_Client(const CGameFramework_Client& rhs) = delete;
	CGameFramework_Client& operator=(const CGameFramework_Client& rhs) = delete;
	~CGameFramework_Client();

	virtual bool Initialize() override;

private:
	virtual void FlushCommandQueue(void);

private:
	virtual void OnResize() override;
	virtual void Update(CTimer& const gt)override;
	virtual void Draw(CTimer& const gt)override;

	virtual void OnMouseDown(WPARAM btnState, int x, int y)override;
	virtual void OnMouseUp(WPARAM btnState, int x, int y)override;
	virtual void OnMouseMove(WPARAM btnState, int x, int y)override;

	void OnKeyboardInput(const CTimer& gt);
	void UpdateCamera(const CTimer& gt);
	void UpdateObjectCBs(const CTimer& gt);
	void UpdateMainPassCB(const CTimer& gt);

	void BuildDescriptorHeaps();
	void BuildConstantBufferViews();
	void BuildRootSignature();
	void BuildShadersAndInputLayout();
	void BuildShapeGeometry();
	void BuildPSOs();
	void BuildFrameResources();
	void BuildRenderItems();

	void BuildComponent(void);
	void DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<CGameObject*>& ritems);

private:
	std::vector<std::unique_ptr<FrameResource>> mFrameResources;
	FrameResource* mCurrFrameResource = nullptr;
	int mCurrFrameResourceIndex = 0;

	ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
	ComPtr<ID3D12DescriptorHeap> mCbvHeap = nullptr;

	ComPtr<ID3D12DescriptorHeap> mSrvDescriptorHeap = nullptr;

	std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> mGeometries;
	std::unordered_map<std::string, ComPtr<ID3DBlob>> mShaders;
	std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> mPSOs;

	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;

	// List of all the render items.
	std::vector<std::unique_ptr<RenderItem>> mAllRitems;
	std::vector<std::unique_ptr<CGameObject>> m_vObjects;

	// Render items divided by PSO.
	std::vector<RenderItem*> mOpaqueRitems;
	std::vector<CGameObject*> m_OpaqueObjects;

	PassConstants mMainPassCB;

	UINT mPassCbvOffset = 0;

	bool mIsWireframe = false;

	CCamera m_Camera;


	XMFLOAT3 mEyePos = { 0.0f, 0.0f, 0.0f };
	XMFLOAT4X4 mView = MathHelper::Identity4x4();
	XMFLOAT4X4 mProj = MathHelper::Identity4x4();

	float mTheta = 1.5f*XM_PI;
	float mPhi = 0.2f*XM_PI;
	float mRadius = 15.0f;

	POINT mLastMousePos;
};

