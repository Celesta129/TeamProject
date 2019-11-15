#include "stdafx.h"
#include "D3DApp.h"

CD3DApp::CD3DApp(HINSTANCE hInstance)
{

}

CD3DApp::~CD3DApp()
{
	if(m_d3dDevice != nullptr)
		FlushCommandQueue();
}

CD3DApp* CD3DApp::m_pInstance = nullptr;
CD3DApp * CD3DApp::GetInstance(void)
{
	return m_pInstance;
}

HINSTANCE CD3DApp::AppInst() const
{
	return m_hAppInst;
}

HWND CD3DApp::MainWnd() const
{
	return m_hMainWnd;
}

float CD3DApp::AspectRatio() const
{
	return m_ClientWidth / m_ClientHeight;
}

bool CD3DApp::Get4XMsaaState() const
{
	return m_b4xMsaaState;
}

void CD3DApp::Set4XMsaaState(bool value)
{
	if (m_b4xMsaaState != value)
	{
		m_b4xMsaaState = value;

		// Recreate the swapchain and buffers with new multisample settings.
		CreateSwapChain();
		OnResize();
	}
}

int CD3DApp::Run()
{
	MSG msg = { 0 };

	m_pTimer->Reset();

	while (msg.message != WM_QUIT)
	{
		// If there are Window messages then process them.
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// Otherwise, do animation/game stuff.
		else
		{
			m_pTimer->Tick();

			if (!m_bAppPaused)
			{
				CalculateFrameStats();
				Update(m_pTimer);
				Draw(m_pTimer);
			}
			else
			{
				Sleep(100);
			}
		}
	}

	return (int)msg.wParam;
}

bool CD3DApp::Initialize()
{
	if (!InitMainWindow())
		return false;

	if (!InitDirect3D())
		return false;

	// Do the initial resize code.
	// ���⼭ ����Ÿ�� ��, ����/���ٽ� �並 ����
	OnResize();

	return true;
}

void CD3DApp::CreateRtvAndDsvDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.NumDescriptors = m_iSwapChainBufferCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;
	ThrowIfFailed(m_d3dDevice->CreateDescriptorHeap(
		&rtvHeapDesc, IID_PPV_ARGS(m_RtvHeap.GetAddressOf())));


	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	ThrowIfFailed(m_d3dDevice->CreateDescriptorHeap(
		&dsvHeapDesc, IID_PPV_ARGS(m_DsvHeap.GetAddressOf())));
}

void CD3DApp::OnResize()
{
	assert(m_d3dDevice);
	assert(m_dxgiSwapChain);
	assert(m_CommandAllocator);

	// Flush before changing any resources.
	FlushCommandQueue();

	ThrowIfFailed(m_GraphicsCommandList->Reset(m_CommandAllocator.Get(), nullptr));

	// Release the previous resources we will be recreating.
	for (int i = 0; i < m_iSwapChainBufferCount; ++i)
		m_SwapChainBuffer[i].Reset();
	m_DepthStencilBuffer.Reset();

	// Resize the swap chain.
	ThrowIfFailed(m_dxgiSwapChain->ResizeBuffers(
		m_iSwapChainBufferCount,
		m_ClientWidth, m_ClientHeight,
		m_BackBufferFormat,
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

	m_CurrentBackBuffer = 0;

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(m_RtvHeap->GetCPUDescriptorHandleForHeapStart());
	for (UINT i = 0; i < m_iSwapChainBufferCount; i++)
	{
		ThrowIfFailed(m_dxgiSwapChain->GetBuffer(i, IID_PPV_ARGS(&m_SwapChainBuffer[i])));
		m_d3dDevice->CreateRenderTargetView(m_SwapChainBuffer[i].Get(), nullptr, rtvHeapHandle);
		rtvHeapHandle.Offset(1, m_RtvDescriptorSize);
	}

	// Create the depth/stencil buffer and view.
	D3D12_RESOURCE_DESC depthStencilDesc;
	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Alignment = 0;
	depthStencilDesc.Width = m_ClientWidth;
	depthStencilDesc.Height = m_ClientHeight;
	depthStencilDesc.DepthOrArraySize = 1;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.Format = m_DepthStencilFormat;
	depthStencilDesc.SampleDesc.Count = m_b4xMsaaState ? 4 : 1;
	depthStencilDesc.SampleDesc.Quality = m_b4xMsaaState ? (m_n4xMsaaQuality - 1) : 0;
	depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE optClear;
	optClear.Format = m_DepthStencilFormat;
	optClear.DepthStencil.Depth = 1.0f;
	optClear.DepthStencil.Stencil = 0;
	ThrowIfFailed(m_d3dDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&depthStencilDesc,
		D3D12_RESOURCE_STATE_COMMON,
		&optClear,
		IID_PPV_ARGS(m_DepthStencilBuffer.GetAddressOf())));

	// Create descriptor to mip level 0 of entire resource using the format of the resource.
	m_d3dDevice->CreateDepthStencilView(m_DepthStencilBuffer.Get(), nullptr, DepthStencilView());

	// Transition the resource from its initial state to be used as a depth buffer.
	m_GraphicsCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_DepthStencilBuffer.Get(),
		D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));

	// Execute the resize commands.
	ThrowIfFailed(m_GraphicsCommandList->Close());
	ID3D12CommandList* cmdsLists[] = { m_GraphicsCommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// Wait until resize is complete.
	FlushCommandQueue();

	// Update the viewport transform to cover the client area.
	m_ViewPort.TopLeftX = 0;
	m_ViewPort.TopLeftY = 0;
	m_ViewPort.Width = static_cast<float>(m_ClientWidth);
	m_ViewPort.Height = static_cast<float>(m_ClientHeight);
	m_ViewPort.MinDepth = 0.0f;
	m_ViewPort.MaxDepth = 1.0f;

	m_ScissorRect = { 0, 0, m_ClientWidth, m_ClientHeight };
}
LRESULT CALLBACK
MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Forward hwnd on because we can get messages (e.g., WM_CREATE)
	// before CreateWindow returns, and thus before mhMainWnd is valid.
	return CD3DApp::GetInstance()->MsgProc(hwnd, msg, wParam, lParam);
}
bool CD3DApp::InitMainWindow()
{
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hAppInst;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"MainWnd";

	if (!RegisterClass(&wc))
	{
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return false;
	}

	// Compute window rectangle dimensions based on requested client area dimensions.
	RECT R = { 0, 0, m_ClientWidth, m_ClientHeight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width = R.right - R.left;
	int height = R.bottom - R.top;

	m_hMainWnd = CreateWindow(L"MainWnd", m_MainWndCaption.c_str(),
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, m_hAppInst, 0);
	if (!m_hMainWnd)
	{
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
		return false;
	}

	ShowWindow(m_hMainWnd, SW_SHOW);
	UpdateWindow(m_hMainWnd);

	return true;
}

bool CD3DApp::InitDirect3D()
{
	// 1. CreateDevice
	// 2. Fence��ü�� �����ϰ� ������ ũ�⸦ ��´�
	// 3. 4xMsaa ǰ�� ���� ���� ���� ����
	// 4. ��� ��⿭, ��� ��� �Ҵ���, �� ��� ��� ����
	// 5. ��ȯ�罽 ����
	// 6. �������α׷��� �ʿ��� ������ �� ����
	// 7. �ĸ� ���� ũ�⸦ ����, �ĸ���ۿ� ���� ������� �並 ����
	// 8. ����/���ٽ� ���۸� �����ϰ� ����/���ٽ� �並 ����
	// 9. ����Ʈ�� ������Ʈ ����
#if defined(DEBUG) || defined(_DEBUG) 
	// Enable the D3D12 debug layer.
	{
		ComPtr<ID3D12Debug> debugController;
		ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
		debugController->EnableDebugLayer();
	}
#endif
	//ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&m_dxgiFactory)));
	ThrowIfFailed(CreateDXGIFactory(IID_PPV_ARGS(&m_dxgiFactory)));

	HRESULT hardwareResult = E_FAIL;
	IDXGIAdapter1 *pd3dAdapter = NULL;
	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != m_dxgiFactory->EnumAdapters1(i, &pd3dAdapter); i++)
	{
		DXGI_ADAPTER_DESC1 dxgiAdapterDesc;
		pd3dAdapter->GetDesc1(&dxgiAdapterDesc);
		if (dxgiAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;
		
		hardwareResult = D3D12CreateDevice(
			pd3dAdapter,
			D3D_FEATURE_LEVEL_12_0,
			IID_PPV_ARGS(&m_d3dDevice));

		if (SUCCEEDED(hardwareResult))  break;
		// ��� �ϵ���� ����Ϳ� ���Ͽ� Ư������ 12�� �����ϴ� �ϵ���� ����̽� ����
	}
	//// Try to create hardware device.
	// HRESULT hardwareResult = D3D12CreateDevice(
	//	nullptr,             // default adapter
	//	D3D_FEATURE_LEVEL_12_0,
	//	IID_PPV_ARGS(&m_d3dDevice));

	// Fallback to WARP device.
	if (FAILED(hardwareResult))
	{
		ComPtr<IDXGIAdapter> pWarpAdapter;
		ThrowIfFailed(m_dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)));

		ThrowIfFailed(D3D12CreateDevice(
			pWarpAdapter.Get(),
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(&m_d3dDevice)));
	}

	ThrowIfFailed(m_d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE,
		IID_PPV_ARGS(&m_Fence)));

	m_RtvDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_DsvDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	m_CbvSrvUavDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// Check 4X MSAA quality support for our back buffer format.
	// All Direct3D 11 capable devices support 4X MSAA for all render 
	// target formats, so we only need to check quality support.

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
	msQualityLevels.Format = m_BackBufferFormat;
	msQualityLevels.SampleCount = 4;
	msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	msQualityLevels.NumQualityLevels = 0;
	ThrowIfFailed(m_d3dDevice->CheckFeatureSupport(
		D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&msQualityLevels,
		sizeof(msQualityLevels)));

	m_n4xMsaaQuality = msQualityLevels.NumQualityLevels;
	assert(m_n4xMsaaQuality > 0 && "Unexpected MSAA quality level.");

#ifdef _DEBUG
	LogAdapters();
#endif

	CreateCommandObjects();
	CreateSwapChain();
	CreateRtvAndDsvDescriptorHeaps();

	return true;
}

void CD3DApp::CreateCommandObjects()
{
	// ��� ��⿭ ������ ����
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	// ��� ��⿭ ����
	ThrowIfFailed(m_d3dDevice->CreateCommandQueue(
		&queueDesc, IID_PPV_ARGS(&m_CommandQueue)
	));

	// ��� �Ҵ��� ����
	ThrowIfFailed(m_d3dDevice->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(m_CommandAllocator.GetAddressOf())
	));

	// ��ɸ���Ʈ ����
	ThrowIfFailed(m_d3dDevice->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		m_CommandAllocator.Get(),		// ������ ��� �Ҵ���
		nullptr,				// �ʱ� ���������� ���� ��ü
		IID_PPV_ARGS(m_GraphicsCommandList.GetAddressOf())
	));

	//�������·� �����ؼ� ���� ��� ����� ó�� �����Ҷ� Reset�� ȣ���Ѵ�.
	// Reset�� ȣ���Ϸ��� ��ɸ���� �����־���Ѵ�.
	m_GraphicsCommandList->Close();
}

void CD3DApp::CreateSwapChain()
{
	// ���� ����ü�� ����
	m_dxgiSwapChain.Reset();

	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = m_ClientWidth;
	sd.BufferDesc.Height = m_ClientHeight;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = m_BackBufferFormat;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SampleDesc.Count = m_b4xMsaaState ? 4 : 1;
	sd.SampleDesc.Quality = m_b4xMsaaState ? (m_n4xMsaaQuality - 1) : 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = m_iSwapChainBufferCount;
	sd.OutputWindow = m_hMainWnd;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	// ��ȯ�罽�� ��ɴ�⿭��  �̿��ؼ� flush�� �����Ѵ�.

	ThrowIfFailed(m_dxgiFactory->CreateSwapChain(
		m_CommandQueue.Get(),
		&sd,
		m_dxgiSwapChain.GetAddressOf())
	);
}

void CD3DApp::FlushCommandQueue()
{
	m_nFenceValue++;
	ThrowIfFailed(m_CommandQueue->Signal(m_Fence.Get(), m_nFenceValue));

	// Wait until the GPU has completed commands up to this fence point.
	if (m_Fence->GetCompletedValue() < m_nFenceValue)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);

		// Fire event when GPU hits current fence.  
		ThrowIfFailed(m_Fence->SetEventOnCompletion(m_nFenceValue, eventHandle));

		// Wait until the GPU hits current fence event is fired.
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}
}
