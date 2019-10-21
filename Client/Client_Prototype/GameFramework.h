#pragma once

class CScene;
class CCamera;
class CTimer;
class CPlayer;

class CGameFramework
{
private:
	CPlayer* m_pPlayer;
	//마지막으로 마우스 버튼을 클릭할 때의 마우스 커서의 위치이다. 
	POINT m_ptOldCursorPos;
private:
	CTimer* m_pGameTimer;

	_TCHAR m_pszFrameRate[100];
private:
	CScene* m_pScene;
private:
	HINSTANCE m_hInstance;
	HWND m_hWnd;

	int m_nWndClientWidth;
	int m_nWndClientHeight;

	IDXGIFactory4 *m_pdxgiFactory;
	IDXGISwapChain3 *m_pdxgiSwapChain;
	ID3D12Device *m_pd3dDevice;

	bool m_bMsaa4xEnable = false;
	UINT m_nMsaa4xQualityLevels = 0;

	static const UINT m_nSwapChainBuffers = 2;
	UINT m_nSwapChainBufferIndex;

	ID3D12Resource *m_ppd3dRenderTargetBuffers[m_nSwapChainBuffers];
	ID3D12DescriptorHeap *m_pd3dRtvDescriptorHeap;
	UINT m_nRtvDescriptorIncrementSize;

	ID3D12Resource *m_pd3dDepthStencilBuffer;
	ID3D12DescriptorHeap *m_pd3dDsvDescriptorHeap;
	UINT m_nDsvDescriptorIncrementSize;

	ID3D12CommandQueue *m_pd3dCommandQueue;
	ID3D12CommandAllocator *m_pd3dCommandAllocator;
	ID3D12GraphicsCommandList *m_pd3dCommandList;

	ID3D12PipelineState *m_pd3dPipelineState;

	ID3D12Fence *m_pd3dFence;
	UINT64 m_nFenceValues[m_nSwapChainBuffers];
	HANDLE m_hFenceEvent;

#if defined(_DEBUG)
	ID3D12Debug *m_pd3dDebugController;
#endif
	CCamera* m_pCamera = NULL;

public:
	CGameFramework();
	~CGameFramework();

	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();

	void CreateDirect3DDevice();
	void CreateCommandQueueAndList();
	void CreateSwapChain();
	void CreateRtvAndDsvDescriptorHeaps();		// RenderTarget View / Depth-Stencil View 서술자힙 생성
	void CreateRenderTargetView();
	void CreateDepthStencilView();


	// 게임 객체의 생성/ 소멸
	void BuildObjects();
	void ReleaseObjects();

	// 사용자 입력/애니메이션/렌더링
	void ProcessInput();
	void AnimateObjects();
	void FrameAdvance();

	// CPU-GPU 동기화 대기
	void WaitForGpuComplete();

	// 윈도우 메세지 (마우스, 키보드 입력) 처리
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	// 전체화면 전환
	void ChangeSwapChainState();

public:
	void MoveToNextFrame();
};