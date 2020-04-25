// Client_Prototype.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//
#include "stdafx.h"
#include "Client_Prototype.h"
#include "Component_Manager.h"
#include "Scene.h"

#define MAX_LOADSTRING 100

UINT g_CbvSrvUavDescriptorSize;

// 전역 변수:

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:

int WINAPI wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	CComponent_Manager::GetInstance();
	CGameFramework_Client GameFramework(hInstance);
	
	try {
		if (!GameFramework.Initialize())
			return 0;

		return GameFramework.Run();
	}
	catch(DxException& e){
		HRESULT hr = GameFramework.GetDevice()->GetDeviceRemovedReason();
		MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
		return 0;
	}

	
	return 0;
}

CGameFramework_Client::CGameFramework_Client(HINSTANCE hInstance)
	:CD3DApp(hInstance)
{

}

CGameFramework_Client::~CGameFramework_Client()
{
	Safe_Release(m_pScene);
	int refCount = CComponent_Manager::DestroyInstance();
}

bool CGameFramework_Client::Initialize()
{
	if (!CD3DApp::Initialize())
		return false;
	ThrowIfFailed(m_GraphicsCommandList->Reset(m_CommandAllocator.Get(), nullptr));

	m_pScene = new CScene(m_d3dDevice, m_GraphicsCommandList);
	if (!m_pScene->Initialize()) 
	{
		return false;
	}

	ThrowIfFailed(m_GraphicsCommandList->Close());

	ID3D12CommandList* cmdLists[] = { m_GraphicsCommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);
	FlushCommandQueue();

	return true;
}

void CGameFramework_Client::FlushCommandQueue(void)
{
	CD3DApp::FlushCommandQueue();
}

void CGameFramework_Client::OnResize()
{
	CD3DApp::OnResize();
	
	if (m_pScene)
		m_pScene->OnResize(AspectRatio());
}

void CGameFramework_Client::Update(const CTimer & gt)
{
	float fTimeElapsed = gt.DeltaTime();
	
	OnKeyboardInput(gt);
	
	m_pScene->Update(gt, m_Fence.Get());

}

void CGameFramework_Client::Draw(const CTimer & gt)
{
	
	// 커맨드리스트 리셋
	if (m_pScene)
		m_pScene->ResetCmdList(m_GraphicsCommandList.Get());
	else
		return;
	//ThrowIfFailed(m_GraphicsCommandList->Reset(m_CommandAllocator.Get(), nullptr));

	// 뷰포트와 씨저렉트 설정. 이 작업은 반드시 커맨드리스트가 리셋된 후에  해야함.
	m_GraphicsCommandList->RSSetViewports(1, &m_ViewPort);
	m_GraphicsCommandList->RSSetScissorRects(1, &m_ScissorRect);

	//  리소스 사용에 대한 상태전이 지정
	m_GraphicsCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));


	// 백버퍼와 뎁스/스텐실버퍼 클리어
	m_GraphicsCommandList->ClearRenderTargetView(CurrentBackBufferView(), Colors::LightSteelBlue, 0, nullptr);
	m_GraphicsCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	// 렌더하기 위한 백버퍼를 지정
	m_GraphicsCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());
	
	// 씬 렌더링. 렌더코드는 여기에!
	m_pScene->Render(m_GraphicsCommandList.Get(), ++m_nFenceValue);

	// 리소스 사용에 대한 상태전이 지정
	m_GraphicsCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	// 명령리스트 작성 끝.
	ThrowIfFailed(m_GraphicsCommandList->Close());

	// 커맨드큐에 내가 내린 명령리스트를 집어넣고 실행
	ID3D12CommandList* cmdsLists[] = { m_GraphicsCommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	
	// 백/프론트 버퍼 교체
	ThrowIfFailed(m_dxgiSwapChain->Present(0, 0));
	m_CurrentBackBuffer = (m_CurrentBackBuffer + 1) % m_iSwapChainBufferCount;

	// Add an instruction to the command queue to set a new fence point. 
   // Because we are on the GPU timeline, the new fence point won't be 
   // set until the GPU finishes processing all the commands prior to this Signal().
	m_CommandQueue->Signal(m_Fence.Get(), m_nFenceValue);

}

void CGameFramework_Client::OnMouseDown(WPARAM btnState, int x, int y)
{
	m_LastMousePos.x = x;
	m_LastMousePos.y = y;

	if (m_pScene)
		m_pScene->OnMouseDown(btnState, x, y, m_LastMousePos);

	SetCapture(m_hMainWnd);

}

void CGameFramework_Client::OnMouseUp(WPARAM btnState, int x, int y)
{
	m_LastMousePos.x = x;
	m_LastMousePos.y = y;

	if (m_pScene)
		m_pScene->OnMouseUp(btnState, x, y,m_LastMousePos);

	ReleaseCapture();
}

void CGameFramework_Client::OnMouseMove(WPARAM btnState, int x, int y)
{
	if (m_pScene)
		m_pScene->OnMouseMove(btnState, x, y, m_LastMousePos);

	m_LastMousePos.x = x;
	m_LastMousePos.y = y;
}

void CGameFramework_Client::OnKeyboardInput(const CTimer & gt)
{
	if (m_pScene)
		m_pScene->OnKeyboardInput(gt.DeltaTime());
}
