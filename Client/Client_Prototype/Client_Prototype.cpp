// Client_Prototype.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "Client_Prototype.h"
#include "D3DApp.h"

class CGameFramework_Client : public CD3DApp
{
public:
	CGameFramework_Client(HINSTANCE hInstance);
	~CGameFramework_Client();

	virtual bool Initialize() override;

private:
	virtual void OnResize() override;
	virtual void Update(CTimer* const gt)override;
	virtual void Draw(CTimer* const gt)override;
};

#define MAX_LOADSTRING 100

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

	try {
		CGameFramework_Client GameFramework(hInstance);
		if (!GameFramework.Initialize())
			return 0;

		GameFramework.Run();
	}
	catch(DxException& e){
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
}

bool CGameFramework_Client::Initialize()
{
	return CD3DApp::Initialize();
}

void CGameFramework_Client::OnResize()
{
	CD3DApp::OnResize();
}

void CGameFramework_Client::Update(CTimer * const gt)
{
}

void CGameFramework_Client::Draw(CTimer * const gt)
{
	// 커맨드리스트 리셋
	ThrowIfFailed(m_CommandAllocator->Reset());
	ThrowIfFailed(m_GraphicsCommandList->Reset(m_CommandAllocator.Get(), nullptr));

	//  리소스 사용에 대한 상태전이 지정
	m_GraphicsCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	// 뷰포트와 씨저렉트 설정. 이 작업은 커맨드리스트가 리셋된 후에 반드시 해야함.
	m_GraphicsCommandList->RSSetViewports(1, &m_ViewPort);
	m_GraphicsCommandList->RSSetScissorRects(1, &m_ScissorRect);

	// 백버퍼와 뎁스/스텐실버퍼 클리어
	m_GraphicsCommandList->ClearRenderTargetView(CurrentBackBufferView(), Colors::LightSteelBlue, 0, nullptr);
	m_GraphicsCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	// 렌더하기 위한 백버퍼를 지정
	m_GraphicsCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

	// 리소스 사용에 대한 상태전이 지정
	m_GraphicsCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	// 명령리스트 작성 끝
	ThrowIfFailed(m_GraphicsCommandList->Close());

	// 커맨드큐에 내가 내린 명령리스트를 집어넣고 실행
	ID3D12CommandList* cmdsLists[] = { m_GraphicsCommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// 백/프론트 버퍼 교체
	ThrowIfFailed(m_dxgiSwapChain->Present(0, 0));
	m_CurrentBackBuffer = (m_CurrentBackBuffer + 1) % m_iSwapChainBufferCount;

	// Wait until frame commands are complete.  This waiting is inefficient and is
	// done for simplicity.  Later we will show how to organize our rendering code
	// so we do not have to wait per frame.
	
	// cpu-gpu 동기화를 위한 펜스. 프레임마다 펜스를 기다린다.
	// 요약 -> 이방식은 비효율적임. 추후 수정할거같음
	FlushCommandQueue();
}
