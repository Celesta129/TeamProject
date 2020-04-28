#pragma once
#include "stdafx.h"
#include "resource.h"
#include "D3DApp.h"
#include "FrameResource.h"

#include "Camera.h"
#include "GameObject.h"
#include "Scene.h"
#include "CSocket.h"
#include "Player.h"
//class CScene;

class CShader;

class CGameFramework_Client : public CD3DApp
{
public:
	CGameFramework_Client(HINSTANCE hInstance);
	CGameFramework_Client(const CGameFramework_Client& rhs) = delete;
	CGameFramework_Client& operator=(const CGameFramework_Client& rhs) = delete;
	~CGameFramework_Client();

	virtual bool Initialize() override;
	int rendercount = 0;

	//Socket
	CSocket *m_pSocket;
	int m_client_id = -1;
	CPlayer *m_Player;

	void processPacket(char* buf);
	virtual int Run() override;

private:
	virtual void FlushCommandQueue(void);

private:
	virtual void OnResize() override;
	virtual void Update(const CTimer& gt)override;
	virtual void Draw(const CTimer& gt)override;

	virtual void OnMouseDown(WPARAM btnState, int x, int y)override;
	virtual void OnMouseUp(WPARAM btnState, int x, int y)override;
	virtual void OnMouseMove(WPARAM btnState, int x, int y)override;

	void OnKeyboardInput(const CTimer& gt);

private:
	CScene* m_pScene = nullptr;

	ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
	ComPtr<ID3D12DescriptorHeap> mCbvHeap = nullptr;

	ComPtr<ID3D12DescriptorHeap> mSrvDescriptorHeap = nullptr;

	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;

	PassConstants mMainPassCB;

	POINT m_LastMousePos;
};

