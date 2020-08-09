#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "pch.h"
#include "Timer.h"
#include "CPlayer.h"
#include "CObject.h"
#include "CWeapon.h"
#include "protocol.h"

//global
#define MAX_BUFFER 1024

//struct Point3D {
//	float x, y, z;
//};

gmtl::Point3f Player_map1_Init_pos[MAX_USER] = {
	gmtl::Point3f(-280.f, 0.f, 0.f), gmtl::Point3f(280.f, 0.f, 0.f),
	gmtl::Point3f(0.f, 0.f, -280.f), gmtl::Point3f(0.f, 0.f, 280.f),
	gmtl::Point3f(-210.f, 0.f, 190.f), gmtl::Point3f(190.f, 0.f, -210.f),
	gmtl::Point3f(210.f, 0.f, 190.f), gmtl::Point3f(-190.f, 0.f, -210.f)
};

gmtl::Point3f Weapon_map1_Init_pos[MAX_WEAPON] = {
	gmtl::Point3f(-585.f, 0.f, 570.f), gmtl::Point3f(-410.f, 0.f, -110.f), gmtl::Point3f(-570.f, 0.f, -570.f),
	gmtl::Point3f(-180.f, 0.f, -520.f), gmtl::Point3f(100.f, 0.f, -520.f), gmtl::Point3f(600.f, 0.f, -455.f),
	gmtl::Point3f(485.f, 0.f, -90.f), gmtl::Point3f(600.f, 0.f, 550.f), gmtl::Point3f(-85.f, 0.f, 470.f),
	gmtl::Point3f(130.f, 0.f, 470.f)
};

enum ENUMOP {
	OP_RECV, OP_SEND, OP_ACCEPT,
	OP_FREE, OP_HIT, OP_PICK, OP_WEAPON_ATTACK, OP_REVIVE, OP_RESPAWN_WEAPON, OP_TIME, OP_FLAG_TIME, OP_END, OP_RESTART
};

struct event_type {
	int obj_id;
	ENUMOP event_id;
	high_resolution_clock::time_point wakeup_time;
	char attack_count;

	constexpr bool operator < (const event_type& left) const
	{
		return (wakeup_time > left.wakeup_time);
	}
};

priority_queue<event_type> timer_queue;
mutex timer_lock;

struct OVER_EX
{
	WSAOVERLAPPED overlapped;
	ENUMOP op;
	char messageBuffer[MAX_BUFFER];
	union {
		SOCKET c_socket;
		WSABUF wsabuf;
	};
};

class SOCKETINFO {
public:
	SOCKET m_socket;
	OVER_EX m_recv_over;
	int m_id;
	int m_prev_size;
	char m_packet_buf[MAX_PACKET_SIZE];
	volatile bool m_connected;

	CPlayer* playerinfo;
	char player_id[MAX_NAME_LEN];

	chrono::high_resolution_clock::time_point attack_time;
	char attack_count;

	SOCKETINFO() {
		m_recv_over.wsabuf.len = MAX_BUFFER;
		m_recv_over.wsabuf.buf = m_recv_over.messageBuffer;
		ZeroMemory(&m_recv_over.overlapped, sizeof(WSAOVERLAPPED));
		m_prev_size = 0;
		m_connected = false;

		playerinfo = new CPlayer();
	}
};

SOCKETINFO clients[MAX_USER];
int g_curr_user_id = 0;
SOCKET listen_socket;
HANDLE g_iocp;
CTimer GameTimer;
CObject g_Object[8];
CWeapon g_weapon_list[10];
CFlag g_Flag;
volatile  bool game_start = false;
int g_timer = 120;


void add_timer(int obj_id, ENUMOP op_type, high_resolution_clock::time_point duration, char attack_count = 0)
{
	timer_lock.lock();
	event_type ev{ obj_id, op_type, duration, attack_count };
	timer_queue.push(ev);
	timer_lock.unlock();
}

void send_packet(int user_id, void* p) {
	char* buf = reinterpret_cast<char *>(p);
	OVER_EX *exover = new OVER_EX;

	exover->op = OP_SEND;
	ZeroMemory(&exover->overlapped, sizeof(exover->overlapped));

	exover->wsabuf.buf = exover->messageBuffer;
	exover->wsabuf.len = buf[0];
	memcpy(exover->messageBuffer, buf, buf[0]);

	WSASend(clients[user_id].m_socket, &exover->wsabuf, 1, NULL, 0, &exover->overlapped, NULL);
}

void send_connected_packet(int user_id) {
	sc_packet_login_ok p;
	p.id = user_id;
	p.size = sizeof(p);
	p.type = SC_CONNECTED;
	p.posX = clients[user_id].playerinfo->m_posX;
	p.posY = clients[user_id].playerinfo->m_posY;
	p.posZ = clients[user_id].playerinfo->m_posZ;

	//통째로 보내면 메모리에 안좋으므로 &붙이자
	send_packet(user_id, &p);
}

void send_enter_packet(int user_id, int o_id) {
	sc_packet_enter p;
	p.id = o_id;
	p.size = sizeof(p);
	p.type = SC_ENTER;
	p.posX = clients[o_id].playerinfo->m_posX;
	p.posY = clients[o_id].playerinfo->m_posY;
	p.posZ = clients[o_id].playerinfo->m_posZ;

	//통째로 보내면 메모리에 안좋으므로 &붙이자
	send_packet(user_id, &p);
}

void send_put_weapon_packet(int user_id, char type, char index, const gmtl::Point3f& pos) {
	sc_packet_put_weapon p;
	p.size = sizeof(p);
	p.type = SC_PUT_WEAPON;
	p.weapon_index = index;
	p.weapon_type = type;
	p.x = pos.mData[0];
	p.y = pos.mData[1];
	p.z = pos.mData[2];

	send_packet(user_id, &p);
}

void send_remove_weapon_packet(int user_id, char type, char index) {
	sc_packet_remove_weapon p;
	p.size = sizeof(p);
	p.type = SC_REMOVE_WEAPON;
	p.weapon_index = index;
	p.weapon_type = type;

	send_packet(user_id, &p);
}

void send_pick_weapon_packet(int user_id, char player_id, char type, char index) {
	sc_packet_pick_weapon p;
	p.size = sizeof(p);
	p.type = SC_PICK_WEAPON;
	p.id = player_id;
	p.weapon_index = index;
	p.weapon_type = type;

	send_packet(user_id, &p);
}

void send_unpick_weapon_packet(int user_id, char player_id, char type, char index) {
	sc_packet_unpick_weapon p;
	p.size = sizeof(p);
	p.type = SC_UNPICK_WEAPON;
	p.id = player_id;
	p.weapon_index = index;
	p.weapon_type = type;

	send_packet(user_id, &p);
}

void send_setting_weapon(int user_id) {
	for (int i = 0; i < MAX_WEAPON; ++i) {
		if (clients[user_id].m_connected == true) {
			char type = g_weapon_list[i].weapon_index;
			gmtl::Point3f pos = g_weapon_list[i].pos;
			
			send_put_weapon_packet(user_id, type, i, pos);
		}
	}
	//flag
	if (g_Flag.drop == true)
		send_put_weapon_packet(user_id, 4, 0, g_Flag.pos);
}

void send_update_state(int user_id, int player_id, int hp) {
	sc_packet_update_state p;
	p.size = sizeof(p);
	p.type = SC_UPDATE_STATE;
	p.id = player_id;
	p.hp = hp;

	send_packet(user_id, &p);
}

void send_timer_packet(int user_id, int timer) {
	sc_packet_timer p;
	p.size = sizeof(p);
	p.type = SC_TIMER;
	p.timer = timer;

	send_packet(user_id, &p);
}

void send_flag_timer_packet(int user_id, int o_id, int timer) {
	sc_packet_flag_timer p;
	p.size = sizeof(p);
	p.type = SC_FLAG_TIMER;
	p.id = o_id;
	p.timer = timer;

	send_packet(user_id, &p);
}

void send_win_flag_packet(int user_id) {
	sc_packet_win_flag p;
	p.size = sizeof(p);
	p.type = SC_WIN;

	send_packet(user_id, &p);
}

void send_lose_flag_packet(int user_id) {
	sc_packet_lose_flag p;
	p.size = sizeof(p);
	p.type = SC_LOSE;

	send_packet(user_id, &p);
}

//접속과 함께 weapon spawn, timer값 보내기
void enter_game(int user_id) {
	clients[user_id].m_connected = true;

	for (int i = 0; i < MAX_USER; i++) {
		if (true == clients[i].m_connected)
			if (user_id != i) {
				send_enter_packet(user_id, i);
				send_enter_packet(i, user_id);

				//2명이상 접속
				if (game_start == false) {
					game_start = true;
					add_timer(0, OP_TIME, chrono::high_resolution_clock::now() + 1s);
				}
			}
	}
}

void setting_weapon() {
	for (int j = 0; j < MAX_WEAPON; ++j) {
		g_weapon_list[j].weapon_index = rand() % (MAX_WEAPON_TYPE - 1);
		g_weapon_list[j].pos = Weapon_map1_Init_pos[j];
	}
}

void send_leave_packet(int user_id, int o_id) {
	sc_packet_leave p;
	p.id = o_id;
	p.size = sizeof(p);
	p.type = SC_LEAVE;

	//통째로 보내면 메모리에 안좋으므로 &붙이자
	send_packet(user_id, &p);
}

void send_move_packet(int user_id, int mover) {
	sc_packet_move p;
	p.id = mover;
	p.size = sizeof(p);
	p.type = SC_MOVEMENT;
	p.ani_index = clients[mover].playerinfo->m_Animation_index;
	p.dashed = clients[mover].playerinfo->m_dashed;
	p.x = clients[mover].playerinfo->m_posX;
	p.y = clients[mover].playerinfo->m_posY;
	p.z = clients[mover].playerinfo->m_posZ;
	p.vx = clients[mover].playerinfo->m_velX;
	p.vy = clients[mover].playerinfo->m_velY;
	p.vz = clients[mover].playerinfo->m_velZ;

	//통째로 보내면 메모리에 안좋으므로 &붙이자
	send_packet(user_id, &p);
}

void send_guard_packet(int user_id, int mover, char flag) {
	sc_packet_guard p;
	p.size = sizeof(p);
	p.type = SC_GUARD;
	p.id = mover;
	p.flag = flag;
	p.ani_index = clients[mover].playerinfo->m_Animation_index;

	//통째로 보내면 메모리에 안좋으므로 &붙이자
	send_packet(user_id, &p);
}

void send_motion_packet(int user_id, int mover) {
	sc_packet_motion p;
	p.id = mover;
	p.size = sizeof(p);
	p.type = SC_ATTACK;
	p.ani_index = clients[mover].playerinfo->m_Animation_index;

	//통째로 보내면 메모리에 안좋으므로 &붙이자
	send_packet(user_id, &p);
}

void process_packet(int user_id, char* buf) {
	switch (buf[1])
	{
	case CS_LOGIN: {
		cs_packet_connect* packet = reinterpret_cast<cs_packet_connect*>(buf);

		strcpy_s(clients[user_id].player_id, packet->id);
		printf("%s(%d) 접속완료 \n", clients[user_id].player_id, user_id);

		send_connected_packet(user_id);

		enter_game(user_id);

		send_setting_weapon(user_id);
	}
		break;
	case CS_MOVEMENT: {
		cs_packet_move* packet = reinterpret_cast<cs_packet_move*>(buf);
		
		if (packet->keydown) {
			switch (packet->direction)
			{
			case CS_UP:
				clients[user_id].playerinfo->m_velZ += 1.f;
				break;
			case CS_DOWN:
				clients[user_id].playerinfo->m_velZ -= 1.f;
				break;
			case CS_LEFT:
				clients[user_id].playerinfo->m_velX -= 1.f;
				break;
			case CS_RIGHT:
				clients[user_id].playerinfo->m_velX += 1.f;
				break;
			default:
				cout << "UnKnown Direction from Client move packet!\n";
				DebugBreak;
				exit(-1);
				break;
			}
		}
		else {
			switch (packet->direction)
			{
			case CS_UP:
				clients[user_id].playerinfo->m_velZ -= 1.f;
				break;
			case CS_DOWN:
				clients[user_id].playerinfo->m_velZ += 1.f;
				break;
			case CS_LEFT:
				clients[user_id].playerinfo->m_velX += 1.f;
				break;
			case CS_RIGHT:
				clients[user_id].playerinfo->m_velX -= 1.f;
				break;
			default:
				cout << "UnKnown Direction from Client move packet!\n";
				DebugBreak;
				exit(-1);
				break;
			}
		}

		gmtl::Vec3f vel = gmtl::Vec3f(clients[user_id].playerinfo->m_velX, clients[user_id].playerinfo->m_velY, clients[user_id].playerinfo->m_velZ);

		//키 다운이 없는지 판단하고 ani index 변경
		if (clients[user_id].playerinfo->m_velX != 0.f || clients[user_id].playerinfo->m_velZ != 0.f) {
			clients[user_id].playerinfo->m_Animation_index = ANIM_INDEX::RUN;

			//vel로 방향벡터 생성
			clients[user_id].playerinfo->m_look = gmtl::makeNormal(vel);
		}
		else {
			clients[user_id].playerinfo->m_Animation_index = ANIM_INDEX::IDLE;
		}
	

		for (auto& cl : clients) {
			if (true == cl.m_connected)
				send_move_packet(cl.m_id, user_id);
		}
	}
		break;
	case CS_ATTACK: {
		cs_packet_motion* packet = reinterpret_cast<cs_packet_motion*>(buf);

		if (packet->key == 1) { // 근접공격
			if (packet->count == 1) {
				clients[user_id].playerinfo->m_Animation_index = ANIM_INDEX::BASIC_ATTACK;
				clients[user_id].attack_time = chrono::high_resolution_clock::now();	//공격시작
				clients[user_id].attack_count = 1;
				add_timer(user_id, OP_FREE, clients[user_id].attack_time + 370ms, 1);
				printf("1타호출 \n");
				add_timer(user_id, OP_HIT, clients[user_id].attack_time + 70ms, 0);
			}
			else if (packet->count == 2) {
				clients[user_id].playerinfo->m_Animation_index = ANIM_INDEX::BASIC_ATTACK;
				clients[user_id].attack_count = 2;
				add_timer(user_id, OP_FREE, clients[user_id].attack_time + 1000ms, 2);
				printf("2타호출 \n");
				add_timer(user_id, OP_HIT, clients[user_id].attack_time + 740ms, 0);
			}

			//피격 - 공격 상태 전환
			clients[user_id].playerinfo->m_state = PLAYER_STATE::ATTACK;
		}
		else if (packet->key == 2) { //망치
			clients[user_id].playerinfo->m_Animation_index = ANIM_INDEX::HAMMER_ATTACK;
			clients[user_id].attack_time = chrono::high_resolution_clock::now();	//공격시작
			clients[user_id].attack_count = 1;
			add_timer(user_id, OP_FREE, clients[user_id].attack_time + 1330ms, 1);
			add_timer(user_id, OP_WEAPON_ATTACK, clients[user_id].attack_time + 1340ms, 1);
			add_timer(user_id, OP_HIT, clients[user_id].attack_time + 560ms, 0);

			//피격 - 공격 상태 전환
			clients[user_id].playerinfo->m_state = PLAYER_STATE::ATTACK;
		}
		else if (packet->key == 3) { //검
			if (packet->count == 1) {
				clients[user_id].playerinfo->m_Animation_index = ANIM_INDEX::SWORD_ATTACK;
				clients[user_id].attack_time = chrono::high_resolution_clock::now();	//공격시작
				clients[user_id].attack_count = 1;
				add_timer(user_id, OP_FREE, clients[user_id].attack_time + 500ms, 1);
				printf("검1타호출 \n");
				add_timer(user_id, OP_WEAPON_ATTACK, clients[user_id].attack_time + 510ms, 1);
				add_timer(user_id, OP_HIT, clients[user_id].attack_time + 440ms, 0);
			}
			else if (packet->count == 2) {
				clients[user_id].playerinfo->m_Animation_index = ANIM_INDEX::SWORD_ATTACK;
				clients[user_id].attack_count = 2;
				add_timer(user_id, OP_FREE, clients[user_id].attack_time + 1070ms, 2);
				printf("검2타호출 \n");
				add_timer(user_id, OP_WEAPON_ATTACK, clients[user_id].attack_time + 1080ms, 2);
				add_timer(user_id, OP_HIT, clients[user_id].attack_time + 600ms, 0);
			}

			//피격 - 공격 상태 전환
			clients[user_id].playerinfo->m_state = PLAYER_STATE::ATTACK;
		}
		else if (packet->key == 4) {
			clients[user_id].playerinfo->m_Animation_index = ANIM_INDEX::SNACK_ATTACK;
			clients[user_id].attack_time = chrono::high_resolution_clock::now();	//공격시작
			add_timer(user_id, OP_FREE, clients[user_id].attack_time + 2s, 0);
			add_timer(user_id, OP_WEAPON_ATTACK, clients[user_id].attack_time + 1990ms, 0);

		}

		for (auto& cl : clients) {
			if (true == cl.m_connected)
				send_motion_packet(cl.m_id, user_id);
		}
	}
		break;
	case CS_ITEM: {
		cs_packet_weapon* packet = reinterpret_cast<cs_packet_weapon*>(buf);
		char type = packet->weapon_type;
		char index = packet->weapon_index;

		if (type != WEAPON_FLAG) {
			if (g_weapon_list[index].drop == true) {
				clients[user_id].playerinfo->m_weapon_type = type;
				clients[user_id].playerinfo->m_weapon_index = index;
				clients[user_id].playerinfo->m_Animation_index = ANIM_INDEX::PICK_UP;

				g_weapon_list[index].owner = user_id;
				g_weapon_list[index].drop = false;

				if (type == WEAPON_HAMMER) {
					clients[user_id].playerinfo->m_weapon_count = 2;
				}
				else if (type == WEAPON_SWORD) {
					clients[user_id].playerinfo->m_weapon_count = 4;
				}
				else if (type == WEAPON_SNACK) {
					clients[user_id].playerinfo->m_weapon_count = 1;
				}
				else if (type == WEAPON_BLOCK) {
					clients[user_id].playerinfo->m_weapon_count = 2;
				}

				add_timer(user_id, OP_FREE, chrono::high_resolution_clock::now() + 600ms);

				for (auto& cl : clients) {
					if (cl.m_connected == true) {
						send_pick_weapon_packet(cl.m_id, user_id, type, index);
					}
				}
			}
		}
		else { //깃발
			if (g_Flag.drop == true) {
				clients[user_id].playerinfo->m_flag = true;
				clients[user_id].playerinfo->flag_time = FLAG_LIMIT_TIME;
				g_Flag.drop = false;
				g_Flag.owner = user_id;

				add_timer(user_id, OP_FLAG_TIME, chrono::high_resolution_clock::now() + 1s);

				for (auto& cl : clients) {
					if (cl.m_connected == true) {
						send_pick_weapon_packet(cl.m_id, user_id, type, index);
						//send_remove_weapon_packet(cl.m_id, type, index);
					}
				}
			}
		}


	}
		break;
	case CS_GUARD: {
		cs_packet_motion* packet = reinterpret_cast<cs_packet_motion*>(buf);

		if (packet->key == 1) {//가드활성화
			clients[user_id].playerinfo->m_guard = true;
			clients[user_id].playerinfo->m_Animation_index = ANIM_INDEX::GUARD;
			clients[user_id].playerinfo->m_state = PLAYER_STATE::DEFENSE;
		}
		else {//가드 비활성화
			clients[user_id].playerinfo->m_guard = false;
			add_timer(user_id, OP_FREE, chrono::high_resolution_clock::now() + 600ms, 0);
			clients[user_id].playerinfo->m_state = PLAYER_STATE::NORMAL;
		}

		char flag = clients[user_id].playerinfo->m_guard;

		for (auto& cl : clients) {
			if (true == cl.m_connected)
				send_guard_packet(cl.m_id, user_id, flag);
		}
	}
		break;
	default:
		cout << "UnKnown Packet Type Error!\n";
		DebugBreak();
		exit(-1);
		break;
	}
}

void disconnect(int user_id) {
	clients[user_id].m_connected = false;
	for (auto& cl : clients) {
		if (clients[cl.m_id].m_connected == true)
			send_leave_packet(cl.m_id, user_id);
	}
}

void recv_packet_construct(int user_id, int io_byte) {
	int rest_byte = io_byte;
	char* p = clients[user_id].m_recv_over.messageBuffer;
	int packet_size = 0;

	if (clients[user_id].m_prev_size != 0) packet_size = clients[user_id].m_packet_buf[0];
	while (rest_byte > 0) {
		//새로 패킷을 처리해야할 때
		if (packet_size == 0) packet_size = *p;
		if (packet_size <= rest_byte + clients[user_id].m_prev_size) {
			memcpy(clients[user_id].m_packet_buf + clients[user_id].m_prev_size, p, packet_size - clients[user_id].m_prev_size);
			p += packet_size - clients[user_id].m_prev_size;
			rest_byte -= packet_size - clients[user_id].m_prev_size;
			packet_size = 0;
			process_packet(user_id, clients[user_id].m_packet_buf);
			clients[user_id].m_prev_size = 0;
		}
		else {
			//저번에 받은거 다음위치에다가 rest_byte 복사
			memcpy(clients[user_id].m_packet_buf + clients[user_id].m_prev_size, p, rest_byte);
			clients[user_id].m_prev_size += rest_byte;
			rest_byte = 0;
			p += rest_byte;
		}
	}
}

void worker_thread() {
	while (1) {
		//io que넣기
		DWORD io_byte;
		ULONG_PTR key;
		WSAOVERLAPPED* over;
		GetQueuedCompletionStatus(g_iocp, &io_byte, &key, &over, INFINITE);

		OVER_EX* exover = reinterpret_cast<OVER_EX*>(over);
		int user_id = static_cast<int>(key);

		switch (exover->op)
		{
		case OP_RECV: {
			if (0 == io_byte) {
				disconnect(user_id);
			}
			else {
				recv_packet_construct(user_id, io_byte);
				ZeroMemory(&clients[user_id].m_recv_over.overlapped, sizeof(clients[user_id].m_recv_over.overlapped));
				DWORD flags = 0;
				WSARecv(clients[user_id].m_socket, &clients[user_id].m_recv_over.wsabuf, 1, NULL, &flags, &clients[user_id].m_recv_over.overlapped, NULL);
			}
		}
					  break;
		case OP_SEND:
			if (io_byte == 0) disconnect(user_id);
			delete exover;
			break;
		case OP_ACCEPT: {
			int user_id = g_curr_user_id++;

			SOCKET client_socket = exover->c_socket;

			CreateIoCompletionPort(reinterpret_cast<HANDLE>(client_socket), g_iocp, user_id, 0);
			g_curr_user_id = g_curr_user_id % MAX_USER;
			clients[user_id].m_id = user_id;
			clients[user_id].m_prev_size = 0;
			clients[user_id].m_recv_over.op = OP_RECV;
			ZeroMemory(&clients[user_id].m_recv_over.overlapped, sizeof(clients[user_id].m_recv_over.overlapped));
			clients[user_id].m_recv_over.wsabuf.buf = clients[user_id].m_recv_over.messageBuffer;
			clients[user_id].m_recv_over.wsabuf.len = MAX_BUFFER;
			clients[user_id].m_socket = client_socket;

			//컨텐츠 초기화
			clients[user_id].playerinfo->m_posX = Player_map1_Init_pos[user_id].mData[0];
			clients[user_id].playerinfo->m_posY = Player_map1_Init_pos[user_id].mData[1];
			clients[user_id].playerinfo->m_posZ = Player_map1_Init_pos[user_id].mData[2];
			DWORD flags = 0;

			//로그인 정보 수신
			WSARecv(client_socket, &clients[user_id].m_recv_over.wsabuf, 1, NULL, &flags, &clients[user_id].m_recv_over.overlapped, NULL);

			//접속 완료후 accept 대기
			client_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
			exover->c_socket = client_socket;
			ZeroMemory(&exover->overlapped, sizeof(exover->overlapped));
			AcceptEx(listen_socket, client_socket, exover->messageBuffer, NULL, sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16, NULL, &exover->overlapped);
		}
				break;
		//case OP_HIT:
		//	break;
		default:
			break;
		}
	}
}

//충돌처리
bool CheckCollPlayer(const CPlayer& trg, const CPlayer& src)
{
	gmtl::Point3f trg_min;
	gmtl::Point3f trg_max;

	gmtl::Point3f src_min;
	gmtl::Point3f src_max;

	trg_min.mData[0] = trg.m_posX - trg.m_sizeX;
	trg_min.mData[2] = trg.m_posZ - trg.m_sizeZ;

	trg_max.mData[0] = trg.m_posX + trg.m_sizeX;
	trg_max.mData[2] = trg.m_posZ + trg.m_sizeZ;

	src_min.mData[0] = src.m_posX - src.m_sizeX;
	src_min.mData[2] = src.m_posZ - src.m_sizeZ;

	src_max.mData[0] = src.m_posX + src.m_sizeX;
	src_max.mData[2] = src.m_posZ + src.m_sizeZ;

	if ((trg_min.mData[0] <= src_max.mData[0] && trg_max.mData[0] >= src_min.mData[0]) &&
		(trg_min.mData[2] <= src_max.mData[2] && trg_max.mData[2] >= src_min.mData[2]))
	{
		return true;
	}
	else
	{
		return false;
	}
}

//bool CheckCollBox(const CPlayer& trg, const Point3D& vMax, const Point3D& vMin)
//{
//	Point3D trg_min;
//	Point3D trg_max;
//
//	trg_min.x = trg.m_posX - trg.m_sizeX;
//	trg_min.y = trg.m_posY;
//	trg_min.z = trg.m_posZ - trg.m_sizeZ;
//
//	trg_max.x = trg.m_posX + trg.m_sizeX;
//	trg_max.y = trg.m_posY + trg.m_sizeY;
//	trg_max.z = trg.m_posZ + trg.m_sizeZ;
//
//	if((trg_min.x <= vMax.x && trg_max.x >= vMin.x) &&
//		(trg_min.y <= vMax.y && trg_max.y >= vMin.y) &&
//		(trg_min.z <= vMax.z && trg_max.z >= vMin.z))
//	{
//		return true;
//	}
//	else
//	{
//		return false;
//	}
//}
//
//bool CheckCollSphere(const CPlayer& trg, const Point3D& src, const float& radius)
//{
//	Point3D trg_min;
//	Point3D trg_max;
//
//	trg_min.x = trg.m_posX - trg.m_sizeX;
//	trg_min.y = trg.m_posY;
//	trg_min.z = trg.m_posZ - trg.m_sizeZ;
//
//	trg_max.x = trg.m_posX + trg.m_sizeX;
//	trg_max.y = trg.m_posY + trg.m_sizeY;
//	trg_max.z = trg.m_posZ + trg.m_sizeZ;
//
//	float close_x = max(trg_min.x, min(src.x, trg_max.x));
//	float close_y = max(trg_min.y, min(src.y, trg_max.y));
//	float close_z = max(trg_min.z, min(src.z, trg_max.z));
//
//	float fDistance = sqrt((close_x - src.x) * (close_x - src.x) +
//						(close_y - src.y) * (close_y - src.y) +
//						(close_z - src.z) * (close_z - src.z));
//
//	if (fDistance > radius)
//		return false;   //충돌안됨
//	else
//		return true;    //충돌
//}

void mapcollision(int user_id) {
	if (clients[user_id].playerinfo->m_posX >= 640)
		clients[user_id].playerinfo->m_posX = 640;
	else if (clients[user_id].playerinfo->m_posX <= -640)
		clients[user_id].playerinfo->m_posX = -640;

	if (clients[user_id].playerinfo->m_posZ >= 640)
		clients[user_id].playerinfo->m_posZ = 640;
	else if(clients[user_id].playerinfo->m_posZ <= -640)
		clients[user_id].playerinfo->m_posZ = -640;
}

void init_Object() {
	g_Object[0].SetObject(-400, 0, 450, 0);
	g_Object[1].SetObject(400, 0, 450, 0);

	g_Object[2].SetObject(400, 0, -350, 1);
	g_Object[3].SetObject(400, 0, -550, 1);

	g_Object[4].SetObject(-400, 0, -250, 2);
	g_Object[5].SetObject(-400, 0, -400, 2);
	g_Object[6].SetObject(-400, 0, -550, 2);

	g_Object[7].SetObject(0, 0, 0, 3);
}

//temp_collision
bool object_collision(int user_id) {
	//left top bench
	if (clients[user_id].playerinfo->m_posZ >= 320 && clients[user_id].playerinfo->m_posZ <= 510) {
		if (clients[user_id].playerinfo->m_posX >= -560 && clients[user_id].playerinfo->m_posX <= -230)
			return true;
		if (clients[user_id].playerinfo->m_posX >= 240 && clients[user_id].playerinfo->m_posX <= 560)
			return true;
	}
	//right bottom seesaw
	if (clients[user_id].playerinfo->m_posX >= 225 && clients[user_id].playerinfo->m_posX <= 565) {
		if (clients[user_id].playerinfo->m_posZ >= -432 && clients[user_id].playerinfo->m_posZ <= -258)
			return true;

		if (clients[user_id].playerinfo->m_posZ >= -630 && clients[user_id].playerinfo->m_posZ <= -466)
			return true;
	}
	//left bottom horse
	if (clients[user_id].playerinfo->m_posX >= -474 && clients[user_id].playerinfo->m_posX <= -310) {
		if (clients[user_id].playerinfo->m_posZ >= -601 && clients[user_id].playerinfo->m_posZ <= -495)
			return true;
		if (clients[user_id].playerinfo->m_posZ >= -449 && clients[user_id].playerinfo->m_posZ <= -350)
			return true;
		if (clients[user_id].playerinfo->m_posZ >= -296 && clients[user_id].playerinfo->m_posZ <= -196)
			return true;
	}

	//user
	float Amax_x = clients[user_id].playerinfo->m_posX + clients[user_id].playerinfo->m_sizeX;
	float Amin_x = clients[user_id].playerinfo->m_posX - clients[user_id].playerinfo->m_sizeX;
	float Amax_z = clients[user_id].playerinfo->m_posZ + clients[user_id].playerinfo->m_sizeZ;
	float Amin_z = clients[user_id].playerinfo->m_posZ - clients[user_id].playerinfo->m_sizeZ;

	for (int j = 0; j < MAX_USER; ++j) {
		if (clients[j].m_connected == true) {
			float Bmax_x = clients[j].playerinfo->m_posX + clients[j].playerinfo->m_sizeX;
			float Bmin_x = clients[j].playerinfo->m_posX - clients[j].playerinfo->m_sizeX;
			float Bmax_z = clients[j].playerinfo->m_posZ + clients[j].playerinfo->m_sizeZ;
			float Bmin_z = clients[j].playerinfo->m_posZ - clients[j].playerinfo->m_sizeZ;

			if (user_id == j) continue;
			if (Amax_x < Bmin_x || Amin_x > Bmax_x) continue;
			if (Amax_z< Bmin_z || Amin_z > Bmax_z) continue;

			return true;
		}
	}

	return false;
}

int player_hit(int user_id) {

	if (clients[user_id].playerinfo->m_state == PLAYER_STATE::ATTACK) {

		gmtl::Vec3f Look = clients[user_id].playerinfo->m_look;
		gmtl::Point3f Pos(clients[user_id].playerinfo->m_posX, clients[user_id].playerinfo->m_posY, clients[user_id].playerinfo->m_posZ);

		float hit_dist = 10.f;
		int type = clients[user_id].playerinfo->m_weapon_type;
		if (type == WEAPON_SWORD)
			hit_dist = 15.f;
		if (type = WEAPON_HAMMER)
			hit_dist = 20.f;
		
		//user
		float Amax_x = Pos.mData[0] + Look.mData[0] * hit_dist + clients[user_id].playerinfo->m_sizeX;
		float Amin_x = Pos.mData[0] + Look.mData[0] * hit_dist - clients[user_id].playerinfo->m_sizeX;
		float Amax_z = Pos.mData[2] + Look.mData[2] * hit_dist + clients[user_id].playerinfo->m_sizeZ;
		float Amin_z = Pos.mData[2] + Look.mData[2] * hit_dist - clients[user_id].playerinfo->m_sizeZ;

		for (int j = 0; j < MAX_USER; ++j) {
			if (clients[j].m_connected == true) {
				if (clients[j].playerinfo->m_guard == true) {
					if (clients[j].playerinfo->m_weapon_type != WEAPON_SWORD) {
						gmtl::Vec3f trg_Look = clients[user_id].playerinfo->m_look;
						
						if ((trg_Look.mData[0] + Look.mData[0] == 0) && (trg_Look.mData[1] + Look.mData[1] == 0) && (trg_Look.mData[2] + Look.mData[2] == 0)) {
							return -1;
						}
					}
				}

				float Bmax_x = clients[j].playerinfo->m_posX + clients[j].playerinfo->m_sizeX;
				float Bmin_x = clients[j].playerinfo->m_posX - clients[j].playerinfo->m_sizeX;
				float Bmax_z = clients[j].playerinfo->m_posZ + clients[j].playerinfo->m_sizeZ;
				float Bmin_z = clients[j].playerinfo->m_posZ - clients[j].playerinfo->m_sizeZ;

				if (user_id == j) continue;
				if (Amax_x < Bmin_x || Amin_x > Bmax_x) continue;
				if (Amax_z< Bmin_z || Amin_z > Bmax_z) continue;

				return j;
			}
		}
	}

	return -1;
}

bool flag_collision(int user_id) {
	gmtl::Point3f Pos(clients[user_id].playerinfo->m_posX, clients[user_id].playerinfo->m_posY, clients[user_id].playerinfo->m_posZ);

	//user
	float Amax_x = Pos.mData[0] + clients[user_id].playerinfo->m_sizeX;
	float Amin_x = Pos.mData[0] - clients[user_id].playerinfo->m_sizeX;
	float Amax_z = Pos.mData[2] + clients[user_id].playerinfo->m_sizeZ;
	float Amin_z = Pos.mData[2] - clients[user_id].playerinfo->m_sizeZ;

	float Bmax_x = g_Flag.pos.mData[0] + 10.f;
	float Bmin_x = g_Flag.pos.mData[0] - 10.f;
	float Bmax_z = g_Flag.pos.mData[2] + 10.f;
	float Bmin_z = g_Flag.pos.mData[2] - 10.f;

	if (Amax_x < Bmin_x || Amin_x > Bmax_x) return false;
	if (Amax_z< Bmin_z || Amin_z > Bmax_z) return false;

	return true;
}

void timer_process()
{

	while (true) {
		timer_lock.lock();
		if (true == timer_queue.empty()) {
			timer_lock.unlock();
			break;
		}
		auto now_t = high_resolution_clock::now();
		event_type temp_ev = timer_queue.top();
		if (timer_queue.top().wakeup_time > high_resolution_clock::now()) {
			timer_lock.unlock();
			break;
		}
		event_type ev = timer_queue.top();
		timer_queue.pop();
		timer_lock.unlock();
		switch (ev.event_id) {
		case OP_FREE:
		{
			if (clients[ev.obj_id].attack_count != ev.attack_count)
				break;

			clients[ev.obj_id].playerinfo->m_state = PLAYER_STATE::NORMAL;
			clients[ev.obj_id].playerinfo->m_Animation_index = ANIM_INDEX::IDLE;
			clients[ev.obj_id].attack_count = 0;			
		}
		break;
		case OP_HIT:
		{
			//OVER_EX* over = new OVER_EX;
			//over->op = ev.event_id;
			//PostQueuedCompletionStatus(g_iocp, 1, ev.obj_id, &over->overlapped);

			for (int i = 0; i < MAX_USER; ++i) {
				if(clients[i].m_connected == true){
					if (player_hit(i) != -1) {
						int trg = player_hit(i);

						if (clients[i].playerinfo->m_weapon_type == WEAPON_HAMMER) { // 망치로 맞았을때
							if (clients[trg].playerinfo->m_flag) {
								clients[i].playerinfo->m_flag = true;
								clients[i].playerinfo->flag_time = FLAG_LIMIT_TIME;
								clients[trg].playerinfo->m_flag = false;

								add_timer(i, OP_FLAG_TIME, chrono::high_resolution_clock::now());

								for (int j = 0; j < MAX_USER; ++j) {
									if(clients[j].m_connected == true)
										send_pick_weapon_packet(j, i, WEAPON_FLAG, 0);
								}
							}

							clients[i].playerinfo->m_Animation_index = ANIM_INDEX::ANI_STUN;
							std::cout << i << "Player 기절\n";
							clients[i].playerinfo->m_state = PLAYER_STATE::STUN;
							add_timer(i, OP_REVIVE, high_resolution_clock::now() + 5000ms);
						}
						else { // 이외의 장비로 맞았을때
							clients[trg].playerinfo->last_hit = i;
							clients[trg].playerinfo->m_Animation_index = ANIM_INDEX::HIT;
							clients[trg].playerinfo->m_state = PLAYER_STATE::HITTED;
							clients[trg].playerinfo->m_hitted = true;
						}
					}
				}
			}
		}
		break;
		case OP_PICK:
			break;
		case OP_WEAPON_ATTACK:
		{
			//과자 회복
			if (clients[ev.obj_id].playerinfo->m_weapon_type == WEAPON_SNACK) {
				clients[ev.obj_id].playerinfo->m_hp += 20;

				for (int i = 0; i < MAX_USER; ++i) {
					if (clients[i].m_connected == true) {
						send_update_state(i, ev.obj_id, clients[ev.obj_id].playerinfo->m_hp);
					}
				}
			}

			//아이템 사용
			if (clients[ev.obj_id].playerinfo->m_weapon_type != -1) {
				clients[ev.obj_id].playerinfo->m_weapon_count -= 1;
				if (clients[ev.obj_id].playerinfo->m_weapon_count <= 0) {
					char type = clients[ev.obj_id].playerinfo->m_weapon_type;
					char index = clients[ev.obj_id].playerinfo->m_weapon_index;

					g_weapon_list[index].owner = -1;
					clients[ev.obj_id].playerinfo->m_weapon_index = -1;
					clients[ev.obj_id].playerinfo->m_weapon_type = -1;
					add_timer(index, OP_RESPAWN_WEAPON, high_resolution_clock::now() + 15s);

					for (int i = 0; i < MAX_USER; ++i) {
						if (clients[i].m_connected == true) {
							send_unpick_weapon_packet(i, ev.obj_id, type, index);
						}
					}
				}
			}
		}
			break;
		case OP_REVIVE:
		{
			clients[ev.obj_id].playerinfo->m_hp = 100;
			clients[ev.obj_id].playerinfo->m_state = PLAYER_STATE::NORMAL;
			clients[ev.obj_id].playerinfo->m_Animation_index = ANIM_INDEX::GETUP;
			add_timer(ev.obj_id, OP_FREE, high_resolution_clock::now() + 2s, 1);

			for (int j = 0; j < MAX_USER; ++j) {
				if (clients[j].m_connected == true) {
					send_update_state(j, ev.obj_id, clients[ev.obj_id].playerinfo->m_hp);
				}
			}
		}
			break;
		case OP_RESPAWN_WEAPON:
		{
			if (g_weapon_list[ev.obj_id].drop == false && g_weapon_list[ev.obj_id].owner == -1) {
				g_weapon_list[ev.obj_id].weapon_index = rand() % (MAX_WEAPON_TYPE - 1);
				g_weapon_list[ev.obj_id].drop = true;
			}

			for (int i = 0; i < MAX_USER; ++i) {
				if (clients[i].m_connected == true)
					send_put_weapon_packet(i, g_weapon_list[ev.obj_id].weapon_index, ev.obj_id, g_weapon_list[ev.obj_id].pos);
			}
		}
			break;
		case OP_TIME:
		{
			g_timer--;
			//cout << g_timer << "초" << endl;

			if (g_timer > 0) {
				add_timer(0, OP_TIME, chrono::high_resolution_clock::now() + 1s);
			}
			else {
				add_timer(0, OP_END, chrono::high_resolution_clock::now());
			}
		}
			break;
		case OP_FLAG_TIME:
		{
			if (clients[ev.obj_id].playerinfo->m_flag == true) {
				clients[ev.obj_id].playerinfo->flag_time--;

				for (int i = 0; i < MAX_USER; ++i) {
					if (clients[i].m_connected == true) {
						send_flag_timer_packet(i, ev.obj_id, clients[ev.obj_id].playerinfo->flag_time);
					}
				}

				if (clients[ev.obj_id].playerinfo->flag_time > 0) {
					add_timer(ev.obj_id, OP_FLAG_TIME, chrono::high_resolution_clock::now() + 1s);
				}
				else { //끝
					add_timer(0, OP_END, chrono::high_resolution_clock::now());
				}
			}
		}
			break;
		case OP_END:
		{
			for (int i = 0; i < MAX_USER; ++i) {
				if (clients[i].playerinfo->m_flag) {
					send_win_flag_packet(i);
					clients[i].playerinfo->m_Animation_index = ANIM_INDEX::WIN;
				}
				else {
					send_lose_flag_packet(i);
					clients[i].playerinfo->m_Animation_index = ANIM_INDEX::LOSE;
				}
			}

			add_timer(0, OP_RESTART, chrono::high_resolution_clock::now() + 5s);
		}
			break;
		case OP_RESTART:
		{
			//플레이어 초기화
			for (int i = 0; i < MAX_USER; ++i) {
				if (clients[i].m_connected == true) {
					clients[i].attack_count = 0;
					clients[i].playerinfo->m_posX = Player_map1_Init_pos[i].mData[0];
					clients[i].playerinfo->m_posY = Player_map1_Init_pos[i].mData[1];
					clients[i].playerinfo->m_posZ = Player_map1_Init_pos[i].mData[2];

					clients[i].playerinfo->m_hp = 100;
					clients[i].playerinfo->m_state = PLAYER_STATE::NORMAL;
					clients[i].playerinfo->m_Animation_index = ANIM_INDEX::IDLE;
					clients[i].playerinfo->m_dashed = false;

					clients[i].playerinfo->m_weapon_type = -1;
					clients[i].playerinfo->m_weapon_index = -1;
					clients[i].playerinfo->m_weapon_count = 0;

					clients[i].playerinfo->m_flag = false;
					clients[i].playerinfo->last_hit = -1;
					clients[i].playerinfo->flag_time = FLAG_LIMIT_TIME;

					clients[i].playerinfo->m_collide_flag = false;
					clients[i].playerinfo->m_collide_weapon = false;
					clients[i].playerinfo->m_hitted = false;
				}
			}

			//글로벌 변수
			g_timer = 120;
			g_Flag.drop = true;
			g_Flag.owner = -1;
			g_Flag.pos = gmtl::Point3f(0.f, 0.f, 0.f);

			setting_weapon();
			for (int i = 0; i < MAX_WEAPON; ++i) {
				g_weapon_list[i].drop = true;
				g_weapon_list[i].owner = -1;
			}

			//broadcast
			for (int i = 0; i < MAX_USER; ++i) {
				if (clients[i].m_connected == true) {
					for (int j = 0; j < MAX_USER; ++j) {
						if (clients[j].m_connected == true) {
							send_move_packet(i, j);
							send_update_state(i, j, 100);
						}
					}

					send_setting_weapon(i);
				}
			}
			
			add_timer(0, OP_TIME, chrono::high_resolution_clock::now() + 1s);
		}
			break;
		default:
			printf("Event Code Error \n");
			while (true);
		}
	}
	
}

void client_update_process()
{
	for (int i = 0; i < MAX_USER; ++i)
	{
		if (clients[i].m_connected == true)
		{
			if (clients[i].playerinfo->m_hp > 0)
			{
				if (clients[i].playerinfo->m_hitted == true)
				{
					if (clients[i].playerinfo->m_state == PLAYER_STATE::HITTED)
					{
						add_timer(i, OP_FREE, high_resolution_clock::now() + 600ms);

						int hp = clients[i].playerinfo->m_hp;

						if (hp > 0) {
							hp -= 20;

							if (hp <= 0) {
								hp = 0;

								if (clients[i].playerinfo->m_flag) {
									int next_flag = clients[i].playerinfo->last_hit;
									clients[next_flag].playerinfo->m_flag = true;
									clients[next_flag].playerinfo->flag_time = FLAG_LIMIT_TIME;
									clients[i].playerinfo->m_flag = false;

									add_timer(next_flag, OP_FLAG_TIME, chrono::high_resolution_clock::now());

									for (int j = 0; j < MAX_USER; ++i) {
										if (clients[j].m_connected == true)
											send_pick_weapon_packet(j, next_flag, WEAPON_FLAG, 0);
									}
								}

								//사망후 애니메이션
								clients[i].playerinfo->m_Animation_index = ANIM_INDEX::ANI_STUN;
								std::cout << i << "Player 기절\n";
								clients[i].playerinfo->m_state = PLAYER_STATE::STUN;
								//타이머로 5초뒤에 부활
								add_timer(i, OP_REVIVE, high_resolution_clock::now() + 5000ms);

							}
						}

						clients[i].playerinfo->m_hp = hp;
						std::cout << i << "캐릭" << clients[i].playerinfo->m_hp << "체력\n";

						for (int j = 0; j < MAX_USER; ++j) {
							if (clients[j].m_connected == true) {
								send_update_state(j, i, hp);
							}
						}
					}

					clients[i].playerinfo->m_hitted = false;
				}
			}
		}
	}
}

void logic() 
{
	auto FixedDeltaTimeInNano = std::chrono::nanoseconds(int(1000000000.f / float(60)));
	std::chrono::nanoseconds AccumulatedTime(0);
	auto Last = chrono::high_resolution_clock::now();

	while (true) {
		auto Now = std::chrono::high_resolution_clock::now();
		auto Duration = std::chrono::duration_cast<std::chrono::nanoseconds>(Now - Last);
		AccumulatedTime += Duration;
		Last = Now;

		while (AccumulatedTime > FixedDeltaTimeInNano)
		{
			AccumulatedTime -= FixedDeltaTimeInNano;
			float DeltaTime = FixedDeltaTimeInNano.count() * 1e-9f;

			timer_process();

			//이동충돌처리
			for (int i = 0; i < MAX_USER; ++i) {
				if (clients[i].m_connected == true) {
					if (clients[i].playerinfo->m_hp > 0) {
						gmtl::Vec3f Look = clients[i].playerinfo->m_look;

						float movement_x = DeltaTime * Look.mData[0] * 150 * abs(clients[i].playerinfo->m_velX);
						float movement_y = DeltaTime * Look.mData[1] * 150 * abs(clients[i].playerinfo->m_velY);
						float movement_z = DeltaTime * Look.mData[2] * 150 * abs(clients[i].playerinfo->m_velZ);

						clients[i].playerinfo->m_posX += movement_x;
						clients[i].playerinfo->m_posY += movement_y;
						clients[i].playerinfo->m_posZ += movement_z;


						if (object_collision(i) == true) {
							clients[i].playerinfo->m_posX -= movement_x * 1.5f;
							clients[i].playerinfo->m_posY -= movement_y * 1.5f;
							clients[i].playerinfo->m_posZ -= movement_z * 1.5f;
						}

						if (flag_collision(i) == true) {
							clients[i].playerinfo->m_collide_flag = true;
						}
						else {
							clients[i].playerinfo->m_collide_flag = false;
						}

						mapcollision(i);
					}
				}
			}

			//플레이어 충돌정보 갱신
			client_update_process();

			for (int i = 0; i < MAX_USER; ++i) {
				if (clients[i].m_connected == true) {
					for (int j = 0; j < MAX_USER; ++j) {
						if (clients[j].m_connected == true) {
							send_move_packet(i, j);
						}
					}
				}
			}

		}
	}
}

int main()
{
	g_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	
	//initialize clients
	init_Object();
	for (int i = 0; i < MAX_USER; ++i) {
		clients[i].m_connected = false;
	}
	setting_weapon();

	WSADATA WSAData;
	if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0) {
		cout << "Fail to load WSAStartup\n";
		return 0;
	}

	// Accept =============
	listen_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (listen_socket == INVALID_SOCKET) {
		cout << "Error - Invalid socket\n";
		return 0;
	}

	SOCKADDR_IN s_address;
	memset(&s_address, 0, sizeof(s_address));
	s_address.sin_family = AF_INET;
	s_address.sin_port = htons(SERVER_PORT);
	s_address.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	if (::bind(listen_socket, reinterpret_cast<sockaddr *>(&s_address), sizeof(s_address)) == SOCKET_ERROR) {
		cout << "Error - Fail Bind\n";
		closesocket(listen_socket);
		WSACleanup();
		return 0;
	}
	
	if (listen(listen_socket, SOMAXCONN) == SOCKET_ERROR) {
		cout << "Error - Fail Listen\n";
		closesocket(listen_socket);
		WSACleanup();
		return 0;
	}

	CreateIoCompletionPort(reinterpret_cast<HANDLE>(listen_socket), g_iocp, 999, 0);
	SOCKET client_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	OVER_EX accept_over;
	ZeroMemory(&accept_over.overlapped, sizeof(accept_over.overlapped));
	accept_over.op = OP_ACCEPT;
	accept_over.c_socket = client_socket;
	AcceptEx(listen_socket, client_socket, accept_over.messageBuffer, NULL, sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16, NULL, &accept_over.overlapped);

	vector<thread> worker_threads;

	for (int i = 0; i < 4; ++i) worker_threads.push_back(thread{ worker_thread });
	thread logic_thread{ logic };

	for (auto &th : worker_threads) th.join();
	logic_thread.join();

	return 0;
}