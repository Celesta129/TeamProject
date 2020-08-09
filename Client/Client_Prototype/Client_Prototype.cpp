// Client_Prototype.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//
#include "stdafx.h"
#include "Client_Prototype.h"
#include "Component_Manager.h"
#include "ModelObject.h"
#include "Scene.h"
#include "Weapon.h"
#include "UI_Timer.h"

#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")

#define MAX_LOADSTRING 100

UINT g_CbvSrvUavDescriptorSize;

// 전역 변수:
CGameFramework_Client* GameFramework = nullptr;
thread RecvThread;
bool Server_disconnect = true;

//핸들
HWND hMainWnd, hGameWnd;
int state = STATE_LOGIN;
bool Is_Host = false;

//비트맵
HBITMAP Login_Image, Wait_Room_Image;
HBITMAP Lobby_player_1, Lobby_player_2, Lobby_player_3, Lobby_player_4, Lobby_player_5, Lobby_player_6, Lobby_player_7, Lobby_player_8;
HBITMAP Lobby_Player, Lobby_Crown, Lobby_Start, Lobby_Waiting, Login_Button;
HBITMAP Name_Tag[8];

//함수선언
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


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

void LoadBitMap() {
	//로그인
	Login_Image = (HBITMAP)LoadImage(NULL, L"resources/ui/main.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	//대기실
	Wait_Room_Image = (HBITMAP)LoadImage(NULL, L"resources/ui/main.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	//캐릭터칸
	Lobby_player_1 = (HBITMAP)LoadImage(NULL, L"resources/ui/lobby_player_1.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	Lobby_player_2 = (HBITMAP)LoadImage(NULL, L"resources/ui/lobby_player_2.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	Lobby_player_3 = (HBITMAP)LoadImage(NULL, L"resources/ui/lobby_player_3.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	Lobby_player_4 = (HBITMAP)LoadImage(NULL, L"resources/ui/lobby_player_4.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	Lobby_player_5 = (HBITMAP)LoadImage(NULL, L"resources/ui/lobby_player_5.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	Lobby_player_6 = (HBITMAP)LoadImage(NULL, L"resources/ui/lobby_player_6.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	Lobby_player_7 = (HBITMAP)LoadImage(NULL, L"resources/ui/lobby_player_7.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	Lobby_player_8 = (HBITMAP)LoadImage(NULL, L"resources/ui/lobby_player_8.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	//버튼및 UI
	Login_Button = (HBITMAP)LoadImage(NULL, L"resources/ui/Login.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	Lobby_Player = (HBITMAP)LoadImage(NULL, L"resources/ui/lobby_player.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	Lobby_Crown = (HBITMAP)LoadImage(NULL, L"resources/ui/128_crown.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	Lobby_Start = (HBITMAP)LoadImage(NULL, L"resources/ui/lobby_start.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	Lobby_Waiting = (HBITMAP)LoadImage(NULL, L"resources/ui/lobby_waiting.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	for (int i = 0; i < MAX_USER; ++i) {
		Name_Tag[i] = (HBITMAP)LoadImage(NULL, L"resources/ui/name_tag.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
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
			if (index == -1 && type == -1) {
				if (m_Player->collision_weapon(&index)) {
					cout << "무기줍기\n";
					vector<CGameObject*> *pvWeapon = CObject_Manager::GetInstance()->Get_Layer(CObject_Manager::LAYER_WEAPON);
					CWeapon* temp_weapon = (CWeapon*)(*pvWeapon)[index];
					type = temp_weapon->get_Type();
					m_pSocket->sendPacket(CS_ITEM, type, index, 0);
				}
				else if (m_Player->collision_flag()) {
					cout << "깃발줍기\n";
					m_pSocket->sendPacket(CS_ITEM, 4, 0, 0);
				}
				else
				{
					if (attack_count < 2) {
						KEY temp;
						temp.key = KEY_A;
						temp.time = chrono::high_resolution_clock::now();
						if (attack_count == 0) {
							attack_time = temp.time;
							m_Player->Set_status(PLAYER_STATE::ATTACK);
						}
						key_buffer.push_back(temp);

						bool key = KEY_A;
						chrono::high_resolution_clock::time_point start = attack_time;

						auto t = find_if(key_buffer.begin(), key_buffer.end(), [&key, &start](const KEY& k) {
							auto temp1 = chrono::duration_cast<chrono::milliseconds>(k.time - start).count();
							if (temp1 >= chrono::duration_cast<chrono::milliseconds>(0ms).count()) {
								if (temp1 < chrono::duration_cast<chrono::milliseconds>(600ms).count()) {
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
		}
		else {	//맨손아닐때
			if (type == 0) { // 검
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
							if (temp1 < chrono::duration_cast<chrono::milliseconds>(600ms).count()) {
								return true;
							}
						}
						return false;
					});

					if (t != key_buffer.end()) {
						attack_count += 1;
						m_pSocket->sendPacket(CS_ATTACK, 3, attack_count, 0);
					}
				}
			}
			else if (type == 1) { //망치
				m_pSocket->sendPacket(CS_ATTACK, 2, attack_count, 0);
			}
			else if (type == 2) { // 과자
				m_pSocket->sendPacket(CS_ATTACK, 4, attack_count, 0);
			}
		}
		attack_state = true;
	}
	if (GetAsyncKeyState(0x41) == 0 && attack_state)
	{
		attack_state = false;
	}

	if (GetAsyncKeyState(0x53) & 0x8000 && !guard_state) {
		guard_state = true;
		m_pSocket->sendPacket(CS_GUARD, 1, 0, 0);
	}
	if (GetAsyncKeyState(0x53) == 0 && guard_state)
	{
		guard_state = false;
		m_pSocket->sendPacket(CS_GUARD, 0, 0, 0);
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
	vector<CGameObject*>* pvWeapon = CObject_Manager::GetInstance()->Get_Layer(CObject_Manager::LAYER_WEAPON);
	vector<CGameObject*>* pvFlag = CObject_Manager::GetInstance()->Get_Layer(CObject_Manager::LAYER_FLAG);
	CWeapon* pFlag = (CWeapon*)(*pvFlag)[0];
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
	case SC_GUARD:
	{
		sc_packet_guard* p_guard = reinterpret_cast<sc_packet_guard*>(buf);
		int other_id = p_guard->id;
		bool flag = p_guard->flag;

		m_pScene->getplayer(other_id)->Set_guard(flag);
		m_pScene->getplayer(other_id)->SetAnimation_index(p_guard->ani_index);

		if (flag == 1) {
			m_pScene->getplayer(other_id)->setAnimStop(true);
		}
		else {
			m_pScene->getplayer(other_id)->setAnimStop(false);
		}
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
			weapon->set_Invisible(false);
		}
	}
		break;
	case SC_PICK_WEAPON:
	{
		sc_packet_pick_weapon* p_pick_weapon = reinterpret_cast<sc_packet_pick_weapon*>(buf);
		int other_id = p_pick_weapon->id;
		int weapon_index = p_pick_weapon->weapon_index;
		int weapon_type = p_pick_weapon->weapon_type;

		vector<CGameObject*> *pvWeapon = CObject_Manager::GetInstance()->Get_Layer(CObject_Manager::LAYER_WEAPON);
		vector<CGameObject*> *pvFlag = CObject_Manager::GetInstance()->Get_Layer(CObject_Manager::LAYER_FLAG);

		if (weapon_type == 4) {//flag
			CWeapon* flag = (CWeapon*)(*pvFlag)[weapon_index];
			flag->set_Player(m_pScene->getplayer(other_id));
		}
		else {
			CWeapon* weapon = (CWeapon*)(*pvWeapon)[weapon_index];
			weapon->set_Player(m_pScene->getplayer(other_id));
			m_pScene->getplayer(other_id)->SetWeaponType(weapon_type);
			m_pScene->getplayer(other_id)->SetWeaponIndex(weapon_index);
			m_pScene->getplayer(other_id)->SetWeapon_grab(true);
		}
	}
		break;
	case SC_REMOVE_WEAPON:
	{
		sc_packet_remove_weapon* p_remove_weapon = reinterpret_cast<sc_packet_remove_weapon*>(buf);
		int weapon_type = p_remove_weapon->weapon_type;
		int weapon_index = p_remove_weapon->weapon_index;

		vector<CGameObject*> *pvWeapon = CObject_Manager::GetInstance()->Get_Layer(CObject_Manager::LAYER_WEAPON);
		vector<CGameObject*> *pvFlag = CObject_Manager::GetInstance()->Get_Layer(CObject_Manager::LAYER_FLAG);

		if (weapon_type == 4) {//flag
			CWeapon* flag = (CWeapon*)(*pvFlag)[weapon_index];
			flag->set_Invisible(true);
		}
		else {
			CWeapon* weapon = (CWeapon*)(*pvWeapon)[weapon_index];
			weapon->set_Invisible(true);
		}
	}
		break;
	case SC_UNPICK_WEAPON:
	{
		sc_packet_unpick_weapon* p_unpick_weapon = reinterpret_cast<sc_packet_unpick_weapon*>(buf);
		int other_id = p_unpick_weapon->id;
		int weapon_type = p_unpick_weapon->weapon_type;
		int weapon_index = p_unpick_weapon->weapon_index;

		vector<CGameObject*> *pvWeapon = CObject_Manager::GetInstance()->Get_Layer(CObject_Manager::LAYER_WEAPON);

		CWeapon* weapon = (CWeapon*)(*pvWeapon)[weapon_index];
		weapon->set_Player(nullptr);
		weapon->set_Invisible(true);

		m_pScene->getplayer(other_id)->SetWeaponType(-1);
		m_pScene->getplayer(other_id)->SetWeaponIndex(-1);
		m_pScene->getplayer(other_id)->SetWeapon_grab(false);

	}
		break;
	case SC_TIMER:
	{
		sc_packet_timer* p_timer = reinterpret_cast<sc_packet_timer*>(buf);
		int timer = p_timer->timer;

		CUI_Timer* pUI = (CUI_Timer*)CObject_Manager::GetInstance()->Get_Object(CObject_Manager::LAYER_UI, 0);
		pUI->Set_Time(timer);
	}
		break;
	case SC_FLAG_TIMER:
	{
		sc_packet_flag_timer* p_flag_timer = reinterpret_cast<sc_packet_flag_timer*>(buf);
		int other_id = p_flag_timer->id;
		int timer = p_flag_timer->timer;

		cout << other_id+1 << "플레이어 깃발유지" << timer << "남음\n";
	}
		break;
	case SC_WIN:
		//게임승리
		for (int i = 0; i < pvWeapon->size(); ++i)
		{
			CWeapon* pWeapon = (CWeapon*)(*pvWeapon)[i];
			pWeapon->set_Player(nullptr);
		}
		pFlag->set_Player(nullptr);
		printf("게임 승리! \n");
		break;
	case SC_LOSE:
		//게임 패배
		for (int i = 0; i < pvWeapon->size(); ++i)
		{
			CWeapon* pWeapon = (CWeapon*)(*pvWeapon)[i];
			pWeapon->set_Player(nullptr);
		}
		pFlag->set_Player(nullptr);
		printf("게임 패배! \n");
		break;
	default:
		printf("Unknown PACKET type [%d]\n", buf[1]);
		break;
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HDC hdc;
	PAINTSTRUCT ps;
	HFONT myFont;
	HFONT oldFont;
	int mx, my;
	if (state == STATE_GAME) {	//게임 시작 후 
		switch (message)
		{
		case WM_SIZE:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MOUSEMOVE:
		case WM_KEYDOWN:
		case WM_KEYUP:
			GameFramework->OnProcessingWindowMessage(hGameWnd, message, wParam, lParam);
			if ((LOWORD(wParam) == VK_DELETE)) {
				// 게임 강제종료
				//GameFramework->OnDestroy();
				GameFramework->GameOver();

				//RECT rc = { 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT };
				//DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_BORDER;
				//AdjustWindowRect(&rc, dwStyle, FALSE);
				//
				//HWND newwindow = CreateWindowW(szWindowClass, szTitle, dwStyle, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInst, nullptr);
				//hMainWnd = newwindow;
				//ShowWindow(hMainWnd, 10);
				//UpdateWindow(hMainWnd);
			}
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	else if (state == STATE_LOGIN) {
		switch (message)
		{
		case WM_CREATE:
			break;
		case WM_SIZE:
			break;
		case WM_LBUTTONDOWN:
			break;
		case WM_LBUTTONUP:
			mx = LOWORD(lParam);
			my = HIWORD(lParam);
			if (mx > 450 && mx < 830 && my>450 && my < 505) {		//ID입력
				text_id = true;
			}
			else if (mx > 450 && mx < 830 && my>550 && my < 605) {	//IP입력
				text_id = false;
			}
			if (Button_Start->Collsion(mx, my))
			{	// 로그인
				if (SERVER_ON)
				{
					GameFramework->SetIDIP(ID, IP_Adress);

					GameFramework->m_pSocket = new CSocket(GameFramework->m_pid, GameFramework->m_pip);
					if (GameFramework->m_pSocket) {
						if (GameFramework->m_pSocket->init())
						{
							t = thread(recvCallBack);

							cs_packet_connect p_connect;
							p_connect.type = CS_CONNECT;
							p_connect.size = sizeof(cs_packet_connect);
							strcpy_s(p_connect.id, _countof(p_connect.id), GameFramework->m_pid);

							send(GameFramework->m_pSocket->clientSocket, (char *)&p_connect, sizeof(cs_packet_connect), 0);
						}
					}
				}
				state = STATE_MENU;
			}
			InvalidateRect(hWnd, NULL, FALSE);
			break;
		case WM_RBUTTONDOWN:
			break;
		case WM_RBUTTONUP:
			break;
		case WM_MOUSEMOVE:
			break;
		case WM_KEYDOWN:
			if (text_id) {
				if ((LOWORD(wParam) >= 'A' && LOWORD(wParam) <= 'Z') || (LOWORD(wParam) >= '0' && LOWORD(wParam) <= '9')) {
					if (lstrlen(ID) < 10) {
						if (LOWORD(wParam) == 190)
							ID[lstrlen(ID)] = '.';
						else
							ID[lstrlen(ID)] = LOWORD(wParam);
					}
				}
				if (LOWORD(wParam) == 8) {
					if (lstrlen(ID) > 0)
						ID[lstrlen(ID) - 1] = '\0';
				}

			}
			else {
				if ((LOWORD(wParam) >= '0' && LOWORD(wParam) <= '9') || LOWORD(wParam) == 190) { //190: '.'
					if (lstrlen(IP_Adress) < 15) {
						if (LOWORD(wParam) == 190)
							IP_Adress[lstrlen(IP_Adress)] = '.';
						else
							IP_Adress[lstrlen(IP_Adress)] = LOWORD(wParam);
					}

				}
				if (LOWORD(wParam) == 8) {
					if (lstrlen(IP_Adress) > 0)
						IP_Adress[lstrlen(IP_Adress) - 1] = '\0';
				}
			}

			InvalidateRect(hWnd, NULL, FALSE);
			break;
		case WM_KEYUP:
			break;
		case WM_PAINT:
			static HDC MemDC;
			static HBITMAP BackBit, oldBackBit;
			static RECT bufferRT;
			MemDC = BeginPaint(hWnd, &ps);

			GetClientRect(hWnd, &bufferRT);
			hdc = CreateCompatibleDC(MemDC);
			BackBit = CreateCompatibleBitmap(MemDC, bufferRT.right, bufferRT.bottom);
			oldBackBit = (HBITMAP)SelectObject(hdc, BackBit);
			PatBlt(hdc, 0, 0, bufferRT.right, bufferRT.bottom, WHITENESS);

			//그리기
			DrawBitmap(hdc, 0, 0, LogIn_Image);
			DrawButton(hdc, Button_Start);
			myFont = CreateFont(30, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, NULL);
			oldFont = (HFONT)SelectObject(hdc, myFont);
			SetTextAlign(hdc, TA_CENTER);

			TextOut(hdc, 630, 463, ID, lstrlen(ID));
			TextOut(hdc, 630, 563, IP_Adress, lstrlen(IP_Adress));
			SetTextAlign(hdc, TA_LEFT);
			SelectObject(hdc, oldFont);
			DeleteObject(myFont);

			//더블버퍼링 끝
			GetClientRect(hWnd, &bufferRT);
			BitBlt(MemDC, 0, 0, bufferRT.right, bufferRT.bottom, hdc, 0, 0, SRCCOPY);
			SelectObject(hdc, oldBackBit);
			DeleteObject(BackBit);
			DeleteDC(hdc);
			EndPaint(hWnd, &ps);

			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	else if (state == STATE_MENU) {
		switch (message)
		{

		case WM_SIZE:
			break;
		case WM_LBUTTONDOWN:
			break;
		case WM_LBUTTONUP:
			InvalidateRect(hWnd, NULL, FALSE);
			mx = LOWORD(lParam);
			my = HIWORD(lParam);
			if (makeroom) {
				if (Room_Make->Collision1(mx, my) == 1) {
					makeroom = false;
					state = STATE_ROOM;

					if (SERVER_ON)
					{
						char* room_name = WCtoC(Room_Make->name);

						cs_packet_make_room p_make_room;
						p_make_room.size = sizeof(cs_packet_make_room);
						p_make_room.type = CS_MAKE_ROOM;
						strcpy_s(p_make_room.name, _countof(p_make_room.name), room_name);

						send(GameFramework->m_pSocket->clientSocket, (char *)&p_make_room, sizeof(cs_packet_make_room), 0);
						Im_Host = true;
					}
					break;
				}
				if (Room_Make->Collision1(mx, my) == 0) {
					makeroom = false;
					break;
				}
			}
			if (makeroom == false) {
				if (Button_MakeRoom->Collsion(mx, my)) { //방만들기 
					cout << "방만들기" << endl;
					makeroom = true;
					break;
				}
				if (Button_Exit->Collsion(mx, my)) { // 종료
					cout << "종료" << endl;
					//exit(1);
					PostQuitMessage(0);
					break;
				}
				if (Button_Left->Collsion(mx, my)) { //방목록 왼쪽으로 넘기기
					cout << "방목록 왼쪽으로 넘기기" << endl;
					break;
				}
				if (Button_Right->Collsion(mx, my)) { //방목록 오른쪽으로 넘기기
					cout << "방목록 오른쪽으로 넘기기" << endl;
					break;
				}
				if (fix->Collsion(mx, my)) {
					cout << "방목록 새로고침" << endl;
					if (SERVER_ON)
					{
						cs_packet_update_room p_update_room;
						p_update_room.size = sizeof(cs_packet_update_room);
						p_update_room.type = CS_UPDATE_ROOM;

						send(GameFramework->m_pSocket->clientSocket, (char *)&p_update_room, sizeof(cs_packet_update_room), 0);
					}
					for (int i = 0; i < 6; i++)
						Button_Room[i]->render = false;
					break;
				}
				for (int i = 0; i < 6; i++)
				{
					if (Button_Room[i]->Collsion(mx, my)) { //임시 방 버튼 (이후 포문으로 체크)
						cout << "방입장" << i << endl;

						if (SERVER_ON)
						{
							int room_num = Button_Room[i]->room_num;

							cs_packet_attend_room p_attend_room;
							p_attend_room.size = sizeof(cs_packet_attend_room);
							p_attend_room.type = CS_ATTEND_ROOM;
							p_attend_room.room_num = room_num;

							send(GameFramework->m_pSocket->clientSocket, (char *)&p_attend_room, sizeof(cs_packet_attend_room), 0);
							Im_Host = false;
						}

						//state = STATE_ROOM;
						break;
					}
				}
			}
			break;
		case WM_RBUTTONDOWN:
			break;
		case WM_RBUTTONUP:
			break;
		case WM_MOUSEMOVE:
			break;
		case WM_KEYDOWN:
			if (makeroom) {
				if ((LOWORD(wParam) >= 'A' && LOWORD(wParam) <= 'Z') || (LOWORD(wParam) >= '0' && LOWORD(wParam) <= '9')) {
					if (lstrlen(Room_Make->name) < 10) {
						if (LOWORD(wParam) == 190)
							Room_Make->name[lstrlen(Room_Make->name)] = '.';
						else
							Room_Make->name[lstrlen(Room_Make->name)] = LOWORD(wParam);
					}
				}
				if (LOWORD(wParam) == 8) {
					if (lstrlen(Room_Make->name) > 0)
						Room_Make->name[lstrlen(Room_Make->name) - 1] = '\0';
				}

			}
			InvalidateRect(hWnd, NULL, FALSE);
			break;
		case WM_KEYUP:
			break;
		case WM_PAINT:
			static HDC MemDC;
			static HBITMAP BackBit, oldBackBit;
			static RECT bufferRT;
			MemDC = BeginPaint(hWnd, &ps);

			GetClientRect(hWnd, &bufferRT);
			hdc = CreateCompatibleDC(MemDC);
			BackBit = CreateCompatibleBitmap(MemDC, bufferRT.right, bufferRT.bottom);
			oldBackBit = (HBITMAP)SelectObject(hdc, BackBit);
			PatBlt(hdc, 0, 0, bufferRT.right, bufferRT.bottom, WHITENESS);
			//그리기
			DrawBitmap(hdc, 0, 0, Menu_Image);
			DrawButton(hdc, Button_MakeRoom);
			DrawButton(hdc, Button_Exit);
			DrawButton(hdc, Button_Left);
			DrawButton(hdc, Button_Right);
			DrawButton(hdc, fix);
			for (int i = 0; i < 6; i++)
				DrawRoom(hdc, Button_Room[i]);
			if (makeroom)
				DrawRoommake(hdc, Room_Make);
			//더블버퍼링 끝
			GetClientRect(hWnd, &bufferRT);
			BitBlt(MemDC, 0, 0, bufferRT.right, bufferRT.bottom, hdc, 0, 0, SRCCOPY);
			SelectObject(hdc, oldBackBit);
			DeleteObject(BackBit);
			DeleteDC(hdc);
			EndPaint(hWnd, &ps);

			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	else if (state == STATE_ROOM) {
		switch (message)
		{
		case WM_SIZE:
			break;
		case WM_LBUTTONDOWN:
			break;
		case WM_LBUTTONUP:
		{
			InvalidateRect(hWnd, NULL, FALSE);
			mx = LOWORD(lParam);
			my = HIWORD(lParam);
			if (SERVER_ON) {
				if (Im_Host) {
					if (Mode_Left->Collsion(mx, my)) {
						cout << "모드 왼쪽으로 변경" << endl;
						if (myroom.map == 0 || myroom.map == 2) {
							if (myroom.mode == MODE_INDIVIDUAL)
								myroom.mode = MODE_TEAM;
							else if (myroom.mode == MODE_TEAM)
								myroom.mode = MODE_INDIVIDUAL;
						}

						int room_num = myroom.room_num;
						int room_mode = myroom.mode;
						int room_map = myroom.map;
						int room_max_user = myroom.max_user;

						cs_packet_room_option p_room_option;
						p_room_option.size = sizeof(cs_packet_room_option);
						p_room_option.type = CS_OPTION_ROOM;
						p_room_option.room_num = room_num;
						p_room_option.room_mode = myroom.mode;
						p_room_option.room_map = myroom.map;
						p_room_option.room_member = myroom.max_user;

						send(GameFramework->m_pSocket->clientSocket, (char *)&p_room_option, sizeof(cs_packet_room_option), 0);

						break;
					}
					if (Mode_Right->Collsion(mx, my)) {
						cout << "모드 오른쪽으로 변경" << endl;
						if (myroom.map == 0 || myroom.map == 2) {
							if (myroom.mode == MODE_INDIVIDUAL)
								myroom.mode = MODE_TEAM;
							else if (myroom.mode == MODE_TEAM)
								myroom.mode = MODE_INDIVIDUAL;
						}

						int room_num = myroom.room_num;
						int room_mode = myroom.mode;
						int room_map = myroom.map;
						int room_max_user = myroom.max_user;

						cs_packet_room_option p_room_option;
						p_room_option.size = sizeof(cs_packet_room_option);
						p_room_option.type = CS_OPTION_ROOM;
						p_room_option.room_num = room_num;
						p_room_option.room_mode = myroom.mode;
						p_room_option.room_map = myroom.map;
						p_room_option.room_member = myroom.max_user;

						send(GameFramework->m_pSocket->clientSocket, (char *)&p_room_option, sizeof(cs_packet_room_option), 0);
						break;
					}
					if (Num_Left->Collsion(mx, my)) {
						cout << "최대인원 왼쪽으로 변경" << endl;
						myroom.max_user -= 1;
						if (myroom.max_user < 2)
							myroom.max_user = 8;

						int room_num = myroom.room_num;

						cs_packet_room_option p_room_option;
						p_room_option.size = sizeof(cs_packet_room_option);
						p_room_option.type = CS_OPTION_ROOM;
						p_room_option.room_num = room_num;
						p_room_option.room_mode = myroom.mode;
						p_room_option.room_map = myroom.map;
						p_room_option.room_member = myroom.max_user;

						send(GameFramework->m_pSocket->clientSocket, (char *)&p_room_option, sizeof(cs_packet_room_option), 0);
						break;

					}
					if (Num_Right->Collsion(mx, my)) {
						cout << "최대인원 오른쪽으로 변경" << endl;
						myroom.max_user += 1;
						if (myroom.max_user > 8)
							myroom.max_user = 2;

						int room_num = myroom.room_num;

						cs_packet_room_option p_room_option;
						p_room_option.size = sizeof(cs_packet_room_option);
						p_room_option.type = CS_OPTION_ROOM;
						p_room_option.room_num = room_num;
						p_room_option.room_mode = myroom.mode;
						p_room_option.room_map = myroom.map;
						p_room_option.room_member = myroom.max_user;

						send(GameFramework->m_pSocket->clientSocket, (char *)&p_room_option, sizeof(cs_packet_room_option), 0);
						break;

					}
					if (Map_Left->Collsion(mx, my)) {
						cout << "맵 왼쪽으로 변경" << endl;
						myroom.map -= 1;
						if (myroom.map < 0)
							myroom.map = 2;
						if (myroom.map == 1)
							myroom.mode = MODE_KING;
						else myroom.mode = MODE_TEAM;

						int room_num = myroom.room_num;

						cs_packet_room_option p_room_option;
						p_room_option.size = sizeof(cs_packet_room_option);
						p_room_option.type = CS_OPTION_ROOM;
						p_room_option.room_num = room_num;
						p_room_option.room_mode = myroom.mode;
						p_room_option.room_map = myroom.map;
						p_room_option.room_member = myroom.max_user;

						send(GameFramework->m_pSocket->clientSocket, (char *)&p_room_option, sizeof(cs_packet_room_option), 0);
						break;

					}
					if (Map_Right->Collsion(mx, my)) {
						cout << "맵 오른쪽으로 변경" << endl;
						myroom.map += 1;
						if (myroom.map > 2)
							myroom.map = 0;
						if (myroom.map == 1)
							myroom.mode = MODE_KING;
						else myroom.mode = MODE_TEAM;

						int room_num = myroom.room_num;

						cs_packet_room_option p_room_option;
						p_room_option.size = sizeof(cs_packet_room_option);
						p_room_option.type = CS_OPTION_ROOM;
						p_room_option.room_num = room_num;
						p_room_option.room_mode = myroom.mode;
						p_room_option.room_map = myroom.map;
						p_room_option.room_member = myroom.max_user;

						send(GameFramework->m_pSocket->clientSocket, (char *)&p_room_option, sizeof(cs_packet_room_option), 0);
						break;

					}
					if (Room_GameStart->Collsion(mx, my)) {
						cout << "게임시작" << endl;
						if (myroom.mode == MODE_TEAM || myroom.mode == MODE_KING) {
							int team_1 = 0;
							int team_2 = 0;
							for (int i = 0; i < 4; i++) {
								if (character[i]->render) {
									team_1 += 1;
								}
							}
							for (int i = 4; i < 8; i++) {
								if (character[i]->render) {
									team_2 += 1;
								}
							}
							if (!(team_1 >= 1 && team_2 >= 1)) {
								break;
							}
						}
						if (SERVER_ON)
						{
							cs_packet_start_room p_start_room;
							p_start_room.size = sizeof(cs_packet_start_room);
							p_start_room.type = CS_START_ROOM;
							p_start_room.room_num = myroom.room_num;

							send(GameFramework->m_pSocket->clientSocket, (char *)&p_start_room, sizeof(cs_packet_start_room), 0);
						}

						if (!SERVER_ON)
						{
							hdc = BeginPaint(hWnd, &ps);
							DrawBitmap(hdc, 0, 0, Loading_Image);
							EndPaint(hWnd, &ps);

							RECT rc = { 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT };
							DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_BORDER;
							AdjustWindowRect(&rc, dwStyle, FALSE);
							if (played == false)
								hGameWnd = CreateWindowW(szWindowClass, szTitle, dwStyle, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInst, nullptr);
							state = STATE_LOADING;
							ShowWindow(hGameWnd, SW_SHOWDEFAULT);
							ShowWindow(hMainWnd, SW_HIDE);
							if (played == false)
								UpdateWindow(hGameWnd);
							InvalidateRect(hGameWnd, NULL, FALSE);
						}
						break;
					}
				}
				if (myroom.mode == MODE_TEAM || myroom.mode == MODE_KING) {
					if (Button_Team1->Collsion(mx, my)) {
						if (GameFramework->My_ID >= 4 && GameFramework->My_ID < 7)
						{
							cout << "팀1로 변경" << endl;
							int room_num = myroom.room_num;

							cs_packet_room_team_change p_team_change;
							p_team_change.size = sizeof(cs_packet_room_team_change);
							p_team_change.type = CS_TEAM_CHANGE_ROOM;
							p_team_change.room_num = room_num;

							send(GameFramework->m_pSocket->clientSocket, (char *)&p_team_change, sizeof(cs_packet_room_team_change), 0);
						}
						break;
					}
					if (Button_Team2->Collsion(mx, my)) {
						if (GameFramework->My_ID >= 0 && GameFramework->My_ID < 4)
						{
							cout << "팀2로 변경" << endl;
							int room_num = myroom.room_num;

							cs_packet_room_team_change p_team_change;
							p_team_change.size = sizeof(cs_packet_room_team_change);
							p_team_change.type = CS_TEAM_CHANGE_ROOM;
							p_team_change.room_num = room_num;

							send(GameFramework->m_pSocket->clientSocket, (char *)&p_team_change, sizeof(cs_packet_room_team_change), 0);
						}
						break;
					}
				}
				if (Room_Out->Collsion(mx, my)) {
					cout << "방 나가기" << endl;
					for (int i = 0; i < MAX_USER; i++) {
						character[i]->render = false;
						character[i]->master = false;
					}
					for (int i = 0; i < 6; i++)
						Button_Room[i]->render = false;
					int room_num = myroom.room_num;

					cs_packet_room_exit p_room_exit;
					p_room_exit.size = sizeof(cs_packet_room_exit);
					p_room_exit.type = CS_ROOM_EXIT;
					p_room_exit.room_num = room_num;

					send(GameFramework->m_pSocket->clientSocket, (char *)&p_room_exit, sizeof(cs_packet_room_exit), 0);

					state = STATE_MENU;
					break;
				}
				if (attack->Collsion(mx, my)) {
					selected_skill = 0;
					GameFramework->selected_skill = selected_skill;
					break;
				}
				if (speed->Collsion(mx, my)) {
					selected_skill = 1;
					GameFramework->selected_skill = selected_skill;
					break;
				}
				if (health->Collsion(mx, my)) {
					selected_skill = 2;
					GameFramework->selected_skill = selected_skill;
					break;
				}

			}
			//-------------------------------------------

			if (!SERVER_ON) {
				if (Mode_Left->Collsion(mx, my)) {
					cout << "모드 왼쪽으로 변경" << endl;
					if (myroom.map == 0 || myroom.map == 2) {
						if (myroom.mode == MODE_INDIVIDUAL)
							myroom.mode = MODE_TEAM;
						else if (myroom.mode == MODE_TEAM)
							myroom.mode = MODE_INDIVIDUAL;
					}
					break;
				}
				if (Mode_Right->Collsion(mx, my)) {
					cout << "모드 오른쪽으로 변경" << endl;
					if (myroom.map == 0 || myroom.map == 2) {
						if (myroom.mode == MODE_INDIVIDUAL)
							myroom.mode = MODE_TEAM;
						else if (myroom.mode == MODE_TEAM)
							myroom.mode = MODE_INDIVIDUAL;
					}
				}
				if (Num_Left->Collsion(mx, my)) {
					cout << "최대인원 왼쪽으로 변경" << endl;
					myroom.max_user -= 1;
					if (myroom.max_user < 2)
						myroom.max_user = 8;
					break;
				}
				if (Num_Right->Collsion(mx, my)) {
					cout << "최대인원 오른쪽으로 변경" << endl;
					myroom.max_user += 1;
					if (myroom.max_user > 8)
						myroom.max_user = 2;
					break;
				}
				if (Map_Left->Collsion(mx, my)) {
					cout << "맵 왼쪽으로 변경" << endl;
					myroom.map -= 1;
					if (myroom.map < 0)
						myroom.map = 2;
					if (myroom.map == 1)
						myroom.mode = MODE_KING;
					else myroom.mode = MODE_TEAM;
					break;
				}
				if (Map_Right->Collsion(mx, my)) {
					cout << "맵 오른쪽으로 변경" << endl;
					myroom.map += 1;
					if (myroom.map > 2)
						myroom.map = 0;
					if (myroom.map == 1)
						myroom.mode = MODE_KING;
					else myroom.mode = MODE_TEAM;
					break;
				}
				if (Room_GameStart->Collsion(mx, my)) {
					cout << "게임시작" << endl;


					hdc = BeginPaint(hWnd, &ps);
					DrawBitmap(hdc, 0, 0, Loading_Image);
					EndPaint(hWnd, &ps);

					RECT rc = { 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT };
					DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_BORDER;
					AdjustWindowRect(&rc, dwStyle, FALSE);
					if (played == false)
						hGameWnd = CreateWindowW(szWindowClass, szTitle, dwStyle, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInst, nullptr);
					state = STATE_LOADING;
					ShowWindow(hGameWnd, SW_SHOWDEFAULT);
					ShowWindow(hMainWnd, SW_HIDE);
					if (played == false)
						UpdateWindow(hGameWnd);
					InvalidateRect(hGameWnd, NULL, FALSE);

					break;
				}
				if (Button_Team1->Collsion(mx, my)) {
					cout << "팀1로 변경" << endl;

					break;
				}
				if (Button_Team2->Collsion(mx, my)) {
					cout << "팀2로 변경" << endl;

					break;
				}
				if (Room_Out->Collsion(mx, my)) {
					cout << "방 나가기" << endl;
					for (int i = 0; i < MAX_USER; i++) {
						character[i]->render = false;
					}
					state = STATE_MENU;

					break;
				}
				if (attack->Collsion(mx, my)) {
					selected_skill = 0;
					break;
				}
				if (speed->Collsion(mx, my)) {
					selected_skill = 1;
					break;
				}
				if (health->Collsion(mx, my)) {
					selected_skill = 2;
					break;
				}
			}
			break;
		}
		case WM_RBUTTONDOWN:
			break;
		case WM_RBUTTONUP:
			break;
		case WM_MOUSEMOVE:
			break;
		case WM_KEYDOWN:
			InvalidateRect(hWnd, NULL, FALSE);
			break;
		case WM_KEYUP:
			break;
		case WM_PAINT:
			static HDC MemDC;
			static HBITMAP BackBit, oldBackBit;
			static RECT bufferRT;
			MemDC = BeginPaint(hWnd, &ps);

			GetClientRect(hWnd, &bufferRT);
			hdc = CreateCompatibleDC(MemDC);
			BackBit = CreateCompatibleBitmap(MemDC, bufferRT.right, bufferRT.bottom);
			oldBackBit = (HBITMAP)SelectObject(hdc, BackBit);
			PatBlt(hdc, 0, 0, bufferRT.right, bufferRT.bottom, WHITENESS);
			//그리기
			DrawBitmap(hdc, 0, 0, Room_Image);
			DrawButton(hdc, Mode_Left);
			DrawButton(hdc, Num_Left);
			DrawButton(hdc, Map_Left);
			DrawButton(hdc, Mode_Right);
			DrawButton(hdc, Num_Right);
			DrawButton(hdc, Map_Right);
			if (myroom.mode == MODE_TEAM || myroom.mode == MODE_KING) {
				DrawButton(hdc, Button_Team1);
				DrawButton(hdc, Button_Team2);
			}
			else {
				DrawButton(hdc, battleroyale);
			}

			DrawButton(hdc, skill);
			DrawButton(hdc, selecting);

			if (selected_skill == 0) {
				attack->SetPosition(635, 500);
				DrawButton(hdc, attack);
				attack->SetPosition(900, 500);

			}
			else if (selected_skill == 1) {
				speed->SetPosition(635, 500);
				DrawButton(hdc, speed);
				speed->SetPosition(990, 500);
			}
			else if (selected_skill == 2) {
				health->SetPosition(635, 500);
				DrawButton(hdc, health);
				health->SetPosition(1081, 500);
			}
			DrawButton(hdc, attack);
			DrawButton(hdc, speed);
			DrawButton(hdc, health);
			SetTextAlign(hdc, TA_CENTER);
			for (int i = 0; i < 8; i++) {
				DrawCharacter(hdc, character[i]);
			}
			SetTextAlign(hdc, TA_LEFT);

			DrawButton(hdc, Room_Out);
			if (Im_Host)
				DrawButton(hdc, Room_GameStart);


			myFont = CreateFont(30, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, NULL);
			oldFont = (HFONT)SelectObject(hdc, myFont);
			SetBkMode(hdc, TRANSPARENT);
			if (myroom.mode == 0)
				TextOut(hdc, 200, 210, L"팀전", lstrlen(L"팀전"));
			if (myroom.mode == 1)
				TextOut(hdc, 190, 210, L"개인전", lstrlen(L"개인전"));
			if (myroom.mode == 2)
				TextOut(hdc, 190, 210, L"대장전", lstrlen(L"대장전"));


			if (myroom.map == 0) {
				wchar_t str[4] = L"웨하스";
				TextOut(hdc, 190, 355, str, lstrlen(str));	//맵
			}
			if (myroom.map == 1) {
				wchar_t str[4] = L"오레오";
				TextOut(hdc, 190, 355, str, lstrlen(str));	//맵
			}
			if (myroom.map == 2) {
				wchar_t str[4] = L"케이크";
				TextOut(hdc, 190, 355, str, lstrlen(str));	//맵
			}
			wchar_t str1[1];
			_itow(myroom.max_user, str1, 10);
			TextOut(hdc, 225, 500, str1, lstrlen(str1));	//최대인원

			SelectObject(hdc, oldFont);
			DeleteObject(myFont);

			if (everyone_start) {
				DrawBitmap(hdc, 0, 0, Loading_Image);
				RECT rc = { 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT };
				DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_BORDER;
				AdjustWindowRect(&rc, dwStyle, FALSE);
				if (played == false)
					hGameWnd = CreateWindowW(szWindowClass, szTitle, dwStyle, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInst, nullptr);
				state = STATE_LOADING;
				ShowWindow(hGameWnd, SW_SHOWDEFAULT);
				ShowWindow(hMainWnd, SW_HIDE);
				if (played == false)
					UpdateWindow(hGameWnd);
				InvalidateRect(hGameWnd, NULL, FALSE);

				for (int i = 0; i < 8; ++i)
				{
					if (character[i]->render)
					{
						//if (wcscmp(character[i]->name, ID) == 0)
						//{
						//	GameFramework->My_ID = i;
						//	cout << "My_ID : " << i;
						//}
						GameFramework->Characters_ID[i] = character[i]->name;
					}
					else GameFramework->Characters_ID[i] = nullptr;
				}
				everyone_start = false;
			}

			//더블버퍼링 끝
			GetClientRect(hWnd, &bufferRT);
			BitBlt(MemDC, 0, 0, bufferRT.right, bufferRT.bottom, hdc, 0, 0, SRCCOPY);
			SelectObject(hdc, oldBackBit);
			DeleteObject(BackBit);
			DeleteDC(hdc);
			EndPaint(hWnd, &ps);

			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	else if (state == STATE_LOADING) {
		switch (message)
		{
		case WM_CREATE:
			hdc = BeginPaint(hGameWnd, &ps);
			DrawBitmap(hdc, 0, 0, Loading_Image);
			EndPaint(hGameWnd, &ps);
			break;
		case WM_LBUTTONDOWN:
			break;
		case WM_LBUTTONUP:
			break;
		case WM_KEYDOWN:
			InvalidateRect(hWnd, NULL, FALSE);
			break;
		case WM_PAINT:
			hdc = BeginPaint(hGameWnd, &ps);
			DrawBitmap(hdc, 0, 0, Loading_Image);
			EndPaint(hGameWnd, &ps);
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	return 0;
}
