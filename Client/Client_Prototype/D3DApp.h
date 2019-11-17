#pragma once
#include "Timer.h"

class CD3DApp
{
	static CD3DApp* m_pInstance;
protected:
	CD3DApp();
	CD3DApp(const CD3DApp& rhs) = delete;
	CD3DApp& operator=(const CD3DApp& rhs) = delete;
	virtual ~CD3DApp();

public:
	static CD3DApp* GetInstance(void);

	HINSTANCE AppInst() const;
	HWND MainWnd() const;
	float AspectRatio() const;

	bool Get4xMsaaState() const;
	void Set4xMsaaState(bool value);

	UINT Get4xMsaaQuality(void) {
		return m_n4xMsaaQuality;
	}

	DXGI_FORMAT GetBackBufferFormat(void) {
		return m_BackBufferFormat;
	}
	DXGI_FORMAT GetDepthStencilFormat(void) {
		return m_DepthStencilFormat;
	}
	int Run();

	virtual bool Initialize(HINSTANCE hInstance);
	// ���⼭����
	virtual LRESULT MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

protected:
	virtual void CreateRtvAndDsvDescriptorHeaps();
	virtual void OnResize();
	virtual void Update(CTimer* const gt) = 0;
	virtual void Draw(CTimer* const gt) = 0;

	virtual void OnMouseDown(WPARAM btnState, int x, int y) {};
	virtual void OnMouseUp(WPARAM btnState, int x, int y) {};
	virtual void OnMouseMove(WPARAM btnState, int x, int y) {};

protected:
	bool InitMainWindow();
	bool InitDirect3D();
	void CreateCommandObjects();

	void CreateSwapChain();
	void FlushCommandQueue();

	ID3D12Resource* CurrentBackBuffer() const {
		return m_SwapChainBuffer[m_CurrentBackBuffer].Get();
	}
	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const {

		return CD3DX12_CPU_DESCRIPTOR_HANDLE(
			m_RtvHeap->GetCPUDescriptorHandleForHeapStart(),
			m_CurrentBackBuffer,
			m_RtvDescriptorSize
		);
	}
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const {
		return m_DsvHeap->GetCPUDescriptorHandleForHeapStart();
	}
	void CalculateFrameStats();

	void LogAdapters();
	void LogAdapterOutputs(IDXGIAdapter* adapter);
	void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);

protected:

	HINSTANCE m_hAppInst = nullptr;
	HWND m_hMainWnd = nullptr;
	bool m_bAppPaused = false;
	bool m_bMinimized = false;
	bool m_bMaximized = false;		// �ִ�ȭ�ƴ°�?
	bool m_bResizing = false; // â �׵θ��� �����ִ°�?
	bool m_bFullScreenState = false;	// ��üȭ�� Ȱ��ȭ ����

	bool m_b4xMsaaState = false;
	UINT m_n4xMsaaQuality = 0;

	CTimer* m_pTimer = nullptr;

	ComPtr<IDXGIFactory4> m_dxgiFactory = nullptr;
	ComPtr<IDXGISwapChain> m_dxgiSwapChain = nullptr;
	ComPtr<ID3D12Device> m_d3dDevice = nullptr;

	ComPtr<ID3D12Fence> m_Fence = nullptr;
	UINT64 m_nFenceValue = 0;

	ComPtr<ID3D12CommandQueue> m_CommandQueue = nullptr;
	ComPtr<ID3D12CommandAllocator> m_CommandAllocator = nullptr;
	ComPtr<ID3D12GraphicsCommandList> m_GraphicsCommandList = nullptr;

	static const int m_iSwapChainBufferCount = 2;
	int m_CurrentBackBuffer = 0;
	ComPtr<ID3D12Resource> m_SwapChainBuffer[m_iSwapChainBufferCount];
	ComPtr<ID3D12Resource> m_DepthStencilBuffer;

	ComPtr<ID3D12DescriptorHeap> m_RtvHeap;
	ComPtr<ID3D12DescriptorHeap> m_DsvHeap;

	D3D12_VIEWPORT m_ViewPort;
	D3D12_RECT m_ScissorRect;

	UINT m_RtvDescriptorSize = 0;
	UINT m_DsvDescriptorSize = 0;
	UINT m_CbvSrvUavDescriptorSize = 0;

	// �Ļ�Ŭ������ �ڽ��� �����ڿ��� �� ��� �������� �ڽ��� ������ �´� �ʱⰪ��� ������ ��
	std::wstring m_MainWndCaption = L"Client_Prototype";
	D3D_DRIVER_TYPE m_d3dDriverType = D3D_DRIVER_TYPE_HARDWARE;
	DXGI_FORMAT m_BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT m_DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	
	int m_ClientWidth = FRAME_BUFFER_WIDTH;
	int m_ClientHeight = FRAME_BUFFER_HEIGHT;

};

