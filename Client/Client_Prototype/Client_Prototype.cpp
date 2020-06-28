// Client_Prototype.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//
#include "stdafx.h"
#include "Client_Prototype.h"
#include "Component_Manager.h"
#include "ModelObject.h"
#include "Scene.h"
#include "Weapon.h"

#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")

#define MAX_LOADSTRING 100

UINT g_CbvSrvUavDescriptorSize;

// 전역 변수:
CGameFramework_Client* GameFramework = nullptr;
thread RecvThread;
bool Server_disconnect = true;

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
void process_data()
{
	while (Server_disconnect) {
		if (GameFramework == nullptr) return;

		int retval = recv(GameFramework->m_pSocket->clientSocket, GameFramework->m_pSocket->buf, MAX_PACKET_SIZE, 0);

		if (retval == 0 || retval == SOCKET_ERROR)
		{
			closesocket(GameFramework->m_pSocket->clientSocket);
		}

		char* ptr = GameFramework->m_pSocket->buf;

		while (retval > 0) {
			if (GameFramework->m_pSocket->packet_size == 0) GameFramework->m_pSocket->packet_size = ptr[0];
			if (GameFramework->m_pSocket->saved_size + retval >= GameFramework->m_pSocket->packet_size) {
				memcpy(GameFramework->m_pSocket->packetBuffer + GameFramework->m_pSocket->saved_size, ptr, GameFramework->m_pSocket->packet_size - GameFramework->m_pSocket->saved_size);
				GameFramework->processPacket(GameFramework->m_pSocket->packetBuffer);

				ptr += GameFramework->m_pSocket->packet_size - GameFramework->m_pSocket->saved_size;
				retval -= GameFramework->m_pSocket->packet_size - GameFramework->m_pSocket->saved_size;
				GameFramework->m_pSocket->packet_size = 0;
				GameFramework->m_pSocket->saved_size = 0;
			}
			else {
				memcpy(GameFramework->m_pSocket->packetBuffer + GameFramework->m_pSocket->saved_size, ptr, retval);
				GameFramework->m_pSocket->saved_size += retval;
				retval = 0;
			}
		}
	}
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	CComponent_Manager::GetInstance();
	CObject_Manager::GetInstance();
	GameFramework = new CGameFramework_Client (hInstance);
	
	try {
		if (!GameFramework->Initialize())
			return 0;
			
		//sever접속
		char temp_ip[15];
		char temp_id[MAX_NAME_LEN];
		cout << "Input id : ";
		cin >> temp_id;
		cout << "Input ip : ";
		cin >> temp_ip;
		GameFramework->m_pSocket = new CSocket(temp_id, temp_ip);
		if (GameFramework->m_pSocket->init()) {
			cs_packet_connect p_connect;
			strcpy_s(p_connect.id, GameFramework->m_pSocket->m_pid);
			p_connect.type = CS_LOGIN;
			p_connect.size = sizeof(cs_packet_connect);

			send(GameFramework->m_pSocket->clientSocket, (char*)&p_connect, sizeof(cs_packet_connect), 0);
		}
		
		return GameFramework->Run();
	}
	catch(DxException& e){
		HRESULT hr = GameFramework->GetDevice()->GetDeviceRemovedReason();
		MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
		Server_disconnect = false;
		Safe_Delete(GameFramework);
		RecvThread.join();
		return 0;
	}
	
	Safe_Delete(GameFramework);
	RecvThread.join();
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
	refCount = CObject_Manager::DestroyInstance();
}

//객체 초기화
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

	m_pSocket = nullptr;
	m_Player = nullptr;

	ThrowIfFailed(m_GraphicsCommandList->Close());

	ID3D12CommandList* cmdLists[] = { m_GraphicsCommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);
	FlushCommandQueue();
	CTimer asdf;
	return true;
}

void CGameFramework_Client::FlushCommandQueue(void)
{
	CD3DApp::FlushCommandQueue();
}

int CGameFramework_Client::Run()
{
	MSG msg = { 0 };

	m_Timer.Reset();

	RecvThread = thread(process_data);

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
			m_Timer.Tick(60.f);

			if (!m_bAppPaused)
			{
				CalculateFrameStats();
				Update(m_Timer);
				Draw(m_Timer);
			}
			else
			{
				Sleep(100);
			}
		}
	}
	return (int)msg.wParam;
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

	for (int i = 0; i < MAX_USER; ++i) {
		if (m_pScene->m_player[i]->Getconnected())
		{
			CPlayer* status = m_pScene->m_player[i];

			if (status->getAnimStatus() == LOOP_END)
			{
				char status = m_pScene->m_player[i]->Get_Status();
				if (status == PLAYER_STATE::ATTACK) {
					m_pScene->m_player[i]->Set_status(PLAYER_STATE::NORMAL);
					if (i == m_client_id) {
						attack_count = 0;
					}
				}
			}
		}
	}

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

//임시적으로 obj_type send
void CGameFramework_Client::OnKeyboardInput(const CTimer & gt)
{
	if (m_pScene)
		m_pScene->OnKeyboardInput(gt.DeltaTime());

	if (GetAsyncKeyState(VK_LEFT) & 0x8000 && !movement_state[0])
	{
		movement_state[0] = true;
		m_pSocket->sendPacket(CS_MOVEMENT, CS_LEFT, true, 0);
	}
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000 && !movement_state[1])
	{
		movement_state[1] = true;
		m_pSocket->sendPacket(CS_MOVEMENT, CS_RIGHT, true, 0);
	}
	if (GetAsyncKeyState(VK_UP) & 0x8000 && !movement_state[2])
	{
		movement_state[2] = true;
		m_pSocket->sendPacket(CS_MOVEMENT, CS_UP, true, 0);
	}
	if (GetAsyncKeyState(VK_DOWN) & 0x8000 && !movement_state[3])
	{
		movement_state[3] = true;
		m_pSocket->sendPacket(CS_MOVEMENT, CS_DOWN, true, 0);
	}


	if (GetAsyncKeyState(VK_LEFT) == 0 && movement_state[0])
	{
		movement_state[0] = false;
		m_pSocket->sendPacket(CS_MOVEMENT, CS_LEFT, false, 0);
	}
	if (GetAsyncKeyState(VK_RIGHT) == 0 && movement_state[1])
	{
		movement_state[1] = false;
		m_pSocket->sendPacket(CS_MOVEMENT, CS_RIGHT, false, 0);
	}
	if (GetAsyncKeyState(VK_UP) == 0 && movement_state[2])
	{
		movement_state[2] = false;
		m_pSocket->sendPacket(CS_MOVEMENT, CS_UP, false, 0);
	}
	if (GetAsyncKeyState(VK_DOWN) == 0 && movement_state[3])
	{
		movement_state[3] = false;
		m_pSocket->sendPacket(CS_MOVEMENT, CS_DOWN, false, 0);
	}


	if (GetAsyncKeyState(0x41) & 0x8000 && !attack_state)
	{
		int type = m_Player->GetWeapontype();	//무기 종류
		int index = m_Player->GetWeaponIndex();  //무기 번호

		if (m_Player->GetWeapon_grab() == false) {
			if (index == -1 && type == -1 && m_Player->collision_weapon()) {
				cout << "무기줍기\n";
				m_pSocket->sendPacket(CS_ITEM, type, index, 0);
			}
			else
			{
				if (attack_count < 2) {
					KEY temp;
					temp.key = KEY_A;
					temp.time = chrono::high_resolution_clock::now();
					if (attack_count == 0) {
						attack_time = chrono::high_resolution_clock::now();
						m_Player->Set_status(PLAYER_STATE::ATTACK);
					}
					key_buffer.push_back(temp);

					bool key = KEY_A;
					chrono::high_resolution_clock::time_point start = attack_time;

					auto t = find_if(key_buffer.begin(), key_buffer.end(), [&key, &start](const KEY& k) {
						auto temp1 = chrono::duration_cast<chrono::milliseconds>(k.time - start).count();
						if (temp1 >= chrono::duration_cast<chrono::milliseconds>(0ms).count()) {
							if (temp1 < chrono::duration_cast<chrono::milliseconds>(800ms).count()) {
								return true;
							}
						}
						return false;
					});

					if (t != key_buffer.end()) {
						attack_count += 1;
						m_pSocket->sendPacket(CS_ATTACK, 1, attack_count, 0);
					}
				}
			}
		}
		else {	//맨손아닐때

		}
		attack_state = true;
	}
	if (GetAsyncKeyState(0x41) == 0 && attack_state)
	{
		attack_state = false;
	}



	////일정시간동안 아무키도 안눌리면 00 초기화
	for (auto it = key_buffer.begin(); it != key_buffer.end();)
	{
		if (it->time < chrono::high_resolution_clock::now() - 3000ms)
		{
			it = key_buffer.erase(it);
		}
		else {
			it++;
		}
	}

}

void CGameFramework_Client::processPacket(char* buf)
{
	switch (buf[1])
	{
	case SC_CONNECTED:
	{
		sc_packet_login_ok* p_login = reinterpret_cast<sc_packet_login_ok*> (buf);
		m_client_id = p_login->id;
		float tempx = p_login->posX, tempy = p_login->posY , tempz = p_login->posZ;
		//player객체 추가
		Set_Camera(m_client_id);
		m_Player = m_pScene->getplayer(m_client_id);
		m_pScene->getplayer(m_client_id)->SetPos(tempx, tempy, tempz);
		m_pScene->getplayer(m_client_id)->Setconnected(true);

		cout << m_client_id << " : (" << tempx << ", " << tempy << ", " << tempz << ")\n";
	}
		break;
	case SC_ENTER:
	{
		sc_packet_enter* p_enter = reinterpret_cast<sc_packet_enter*>(buf);
		int other_id = p_enter->id;
		float tempx = p_enter->posX, tempy = p_enter->posY, tempz = p_enter->posZ;

		m_pScene->getplayer(other_id)->SetPos(tempx, tempy, tempz);
		m_pScene->getplayer(other_id)->Setconnected(true);
		cout << other_id << " : (" << tempx << ", " << tempy << ", " << tempz << ")\n";
	}
		break;
	case SC_LEAVE: {
		sc_packet_leave* p_leave = reinterpret_cast<sc_packet_leave*>(buf);
		int other_id = p_leave->id;

		m_pScene->getplayer(other_id)->Setconnected(false);
	}
		break;
	case SC_MOVEMENT:
	{
		sc_packet_move* p_movement = reinterpret_cast<sc_packet_move*>(buf);
		int other_id = p_movement->id;
		
		m_pScene->getplayer(other_id)->SetPos(p_movement->x, p_movement->y, p_movement->z);
		m_pScene->getplayer(other_id)->SetVelocity(p_movement->vx, p_movement->vy, p_movement->vz);
		m_pScene->getplayer(other_id)->SetAnimation_index(p_movement->ani_index);

		//cout << p_movement->x << ", " << p_movement->y << ", " << p_movement->z << endl;
		
	}
		break;
	case SC_ATTACK:
	{
		sc_packet_motion* p_motion = reinterpret_cast<sc_packet_motion*>(buf);
		int other_id = p_motion->id;

		m_pScene->getplayer(other_id)->SetAnimation_index(p_motion->ani_index);

	}
		break;
	case SC_UPDATE_STATE:
	{
		sc_packet_update_state* p_state = reinterpret_cast<sc_packet_update_state*>(buf);
		int other_id = p_state->id;
		int player_hp = p_state->hp;

		m_pScene->getplayer(other_id)->Set_HP(player_hp);

	}
		break;
	case SC_PUT_WEAPON: 
	{
		sc_packet_put_weapon* p_put_weapon = reinterpret_cast<sc_packet_put_weapon*>(buf);
		int type = p_put_weapon->weapon_type;
		int index = p_put_weapon->weapon_index;
		int x = p_put_weapon->x;
		int y = p_put_weapon->y;
		int z = p_put_weapon->z;
		cout << type <<","<< index << "," << x << "," << y <<","<< z << endl;

		vector<CGameObject*> *pvWeapon = CObject_Manager::GetInstance()->Get_Layer(CObject_Manager::LAYER_WEAPON);
		vector<CGameObject*> *pvFlag = CObject_Manager::GetInstance()->Get_Layer(CObject_Manager::LAYER_FLAG);

		if (type == 4) {//flag
			(*pvFlag)[index]->Get_Transform()->Set_Pos(XMFLOAT3(x, y, z));
			CWeapon* flag = (CWeapon*)(*pvFlag)[index];
			flag->set_Type(type);
		}
		else {
			(*pvWeapon)[index]->Get_Transform()->Set_Pos(XMFLOAT3(x, y, z));
			CWeapon* weapon = (CWeapon*)(*pvWeapon)[index];
			weapon->set_Type(type);
		}
	}
		break;
	case SC_PICK_WEAPON:
	{
		sc_packet_pick_weapon* p_pick_weapon = reinterpret_cast<sc_packet_pick_weapon*>(buf);
		int other_id = p_pick_weapon->id;
		int weapon_index = p_pick_weapon->weapon_index;
		int weapon_type = p_pick_weapon->weapon_type;
	}
		break;
	case SC_REMOVE_WEAPON:
	{
		sc_packet_remove_weapon* p_remove_weapon = reinterpret_cast<sc_packet_remove_weapon*>(buf);
		int weapon_type = p_remove_weapon->weapon_type;
		int weapon_index = p_remove_weapon->weapon_index;
	}
		break;
	case SC_UNPICK_WEAPON:
	{
		sc_packet_unpick_weapon* p_unpick_weapon = reinterpret_cast<sc_packet_unpick_weapon*>(buf);
		int other_id = p_unpick_weapon->id;
		int type = p_unpick_weapon->weapon_type;
		int index = p_unpick_weapon->weapon_index;
	}
		break;
	case SC_TIMER:
	{
		sc_packet_timer* p_timer = reinterpret_cast<sc_packet_timer*>(buf);
		int timer = p_timer->timer;
	}
		break;
	case SC_FLAG_TIMER:
	{
		sc_packet_flag_timer* p_flag_timer = reinterpret_cast<sc_packet_flag_timer*>(buf);
		int other_id = p_flag_timer->id;
		int timer = p_flag_timer->timer;
	}
		break;
	case SC_WIN:
		//게임승리
		printf("게임 승리! \n");
		break;
	case SC_LOSE:
		//게임 패배
		printf("게임 패배! \n");
		break;
	default:
		printf("Unknown PACKET type [%d]\n", buf[1]);
		break;
	}
}