#pragma once
#include "Timer.h"

class CD3DApp
{
protected:
	CD3DApp(HINSTANCE hInstance);
	CD3DApp(const CD3DApp& rhs) = delete;
	CD3DApp& operator=(const CD3DApp& rhs) = delete;
	virtual ~CD3DApp();

public:
	static CD3DApp* GetInstance(void);

	HINSTANCE AppInst() const;

	HINSTANCE m_hAppInst = nullptr;
	HWND m_hMainWnd = nullptr;
	bool m_bAppPaused = false;
	bool m_bMinimized = false;
	bool m_bMaximized = false;		// 최대화됐는가?
	bool m_bResizing = false; // 창 테두리를 끌고있는가?
	bool m_bFullScreenState = false;	// 전체화면 활성화 여부

	bool m_b4xMsaaState = false;
	UINT m_n4xMsaaQuality = 0;

	CTimer* m_pTimer = nullptr;

	IDXGIFactory4* m_pdxgiFactory = nullptr;
	IDXGISwapChain* m_pdxgiSwapChain = nullptr;
	ID3D12Device* m_pd3dDevice = nullptr;

	ID3D12Fence* m_pFence = nullptr;
	UINT64 m_nFenceValue = 0;

	ID3D12CommandQueue* m_pCommandQueue = nullptr;
	ID3D12CommandAllocator* m_pCommandAllocator = nullptr;
	ID3D12GraphicsCommandList* m_pGraphicsCommandList = nullptr;

	static const int iSwapChainBufferCount = 2;
	int m_CurBackBuffer = 0;
	ID3D12Resource* m_pSwapChainBuffer[iSwapChainBufferCount];
	ID3D12Resource* m_pDepthStencilBuffer;

	ID3D12DescriptorHeap* m_pRtvHeap;
	ID3D12DescriptorHeap* m_pDsvHeap;

	//D3D12_VIEWPOR
};

