#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "pch.h"
#include "Timer.h"
#include "CPlayer.h"
#include "CObject.h"
#include "protocol.h"


//global
#define MAX_BUFFER 1024

//struct Point3D {
//	float x, y, z;
//};

enum ENUMOP { OP_RECV, OP_SEND, OP_ACCEPT };

struct OVER_EX
{
	WSAOVERLAPPED overlapped;
	ENUMOP op;
	char messageBuffer[MAX_BUFFER];
	WSABUF wsabuf;
};

class SOCKETINFO {
public:
	SOCKET m_socket;
	OVER_EX m_recv_over;
	int m_id;
	int m_prev_size;
	char m_packet_buf[MAX_PACKET_SIZE];
	volatile bool m_connected;

	float playerX, playerY, playerZ;
	float velocityX, velocityY, velocityZ;
	char m_ani_index;
	volatile bool m_dashed;
	char player_id[MAX_NAME_LEN];

	chrono::high_resolution_clock::time_point attack_time;
	char attack_count;

	char state;
	

	SOCKETINFO() {
		m_recv_over.wsabuf.len = MAX_BUFFER;
		m_recv_over.wsabuf.buf = m_recv_over.messageBuffer;
		ZeroMemory(&m_recv_over.overlapped, sizeof(WSAOVERLAPPED));
		m_prev_size = 0;
		m_connected = false;

		m_ani_index = 0;
		m_dashed = false;
	}
};

SOCKETINFO clients[MAX_USER];
int g_curr_user_id = 0;
HANDLE g_iocp;
CTimer GameTimer;
CObject g_Object[8];

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
	p.posX = clients[user_id].playerX;
	p.posY = clients[user_id].playerY;
	p.posZ = clients[user_id].playerZ;

	//통째로 보내면 메모리에 안좋으므로 &붙이자
	send_packet(user_id, &p);
}

void send_enter_packet(int user_id, int o_id) {
	sc_packet_enter p;
	p.id = o_id;
	p.size = sizeof(p);
	p.type = SC_ENTER;
	p.posX = clients[o_id].playerX;
	p.posY = clients[o_id].playerY;
	p.posZ = clients[o_id].playerZ;

	//통째로 보내면 메모리에 안좋으므로 &붙이자
	send_packet(user_id, &p);
}

void enter_game(int user_id) {
	clients[user_id].m_connected = true;

	for (int i = 0; i < MAX_USER; i++) {
		if (true == clients[i].m_connected)
			if (user_id != i) {
				send_enter_packet(user_id, i);
				send_enter_packet(i, user_id);
			}
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
	p.ani_index = clients[mover].m_ani_index;
	p.dashed = clients[mover].m_dashed;
	p.x = clients[mover].playerX;
	p.y = clients[mover].playerY;
	p.z = clients[mover].playerZ;
	p.vx = clients[mover].velocityX;
	p.vy = clients[mover].velocityY;
	p.vz = clients[mover].velocityZ;

	//통째로 보내면 메모리에 안좋으므로 &붙이자
	send_packet(user_id, &p);
}

void send_motion_packet(int user_id, int mover) {
	sc_packet_motion p;
	p.id = mover;
	p.size = sizeof(p);
	p.type = SC_ATTACK;
	p.ani_index = clients[mover].m_ani_index;

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
	}
		break;
	case CS_MOVEMENT: {
		cs_packet_move* packet = reinterpret_cast<cs_packet_move*>(buf);
		
		float fSpeed = 0.5f;

		if (packet->keydown) {
			switch (packet->direction)
			{
			case CS_UP:
				clients[user_id].velocityZ += fSpeed;
				break;
			case CS_DOWN:
				clients[user_id].velocityZ -= fSpeed;
				break;
			case CS_LEFT:
				clients[user_id].velocityX -= fSpeed;
				break;
			case CS_RIGHT:
				clients[user_id].velocityX += fSpeed;
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
				clients[user_id].velocityZ -= fSpeed;
				break;
			case CS_DOWN:
				clients[user_id].velocityZ += fSpeed;
				break;
			case CS_LEFT:
				clients[user_id].velocityX += fSpeed;
				break;
			case CS_RIGHT:
				clients[user_id].velocityX -= fSpeed;
				break;
			default:
				cout << "UnKnown Direction from Client move packet!\n";
				DebugBreak;
				exit(-1);
				break;
			}
		}

		//키 다운이 없는지 판단하고 ani index 변경
		if (clients[user_id].velocityX != 0.f || clients[user_id].velocityZ != 0.f) {
			clients[user_id].m_ani_index = 1;
		}
		else {
			clients[user_id].m_ani_index = 0;
		}

		for (auto& cl : clients) {
			if (true == cl.m_connected)
				send_move_packet(cl.m_id, user_id);
		}
	}
		break;
	case CS_ATTACK: {
		cs_packet_motion* packet = reinterpret_cast<cs_packet_motion*>(buf);

		if (packet->key == 1) {
			if (packet->count == 0) {
				clients[user_id].m_ani_index = 2;
				clients[user_id].attack_time = chrono::high_resolution_clock::now();	//공격시작
				clients[user_id].attack_count = 1;
			}
			else if (packet->count == 1) {
				clients[user_id].m_ani_index = 2;
				clients[user_id].attack_count = 2;
			}

			//피격 - 공격 상태 전환
			clients[user_id].state = 1;
		}
		else if (packet->key == 0) {
			clients[user_id].m_ani_index = 0;
			clients[user_id].state = 0;
		}

		for (auto& cl : clients) {
			if (true == cl.m_connected)
				send_motion_packet(cl.m_id, user_id);
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

//충돌처리
bool CheckCollPlayer(const CPlayer& trg, const CPlayer& src)
{
	Point3D trg_min;
	Point3D trg_max;

	Point3D src_min;
	Point3D src_max;

	trg_min.x = trg.m_posX - trg.m_sizeX;
	trg_min.z = trg.m_posZ - trg.m_sizeZ;

	trg_max.x = trg.m_posX + trg.m_sizeX;
	trg_max.z = trg.m_posZ + trg.m_sizeZ;

	src_min.x = src.m_posX - src.m_sizeX;
	src_min.z = src.m_posZ - src.m_sizeZ;

	src_max.x = src.m_posX + src.m_sizeX;
	src_max.z = src.m_posZ + src.m_sizeZ;

	if ((trg_min.x <= src_max.x && trg_max.x >= src_min.x) &&
		(trg_min.z <= src_max.z && trg_max.z >= src_min.z))
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
	if (clients[user_id].playerX >= 640)
		clients[user_id].playerX = 640;
	else if (clients[user_id].playerX <= -640)
		clients[user_id].playerX = -640;

	if (clients[user_id].playerZ >= 640)
		clients[user_id].playerZ = 640;
	else if(clients[user_id].playerZ <= -640)
		clients[user_id].playerZ = -640;
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
	if (clients[user_id].playerZ >= 320 && clients[user_id].playerZ <= 510) {
		if (clients[user_id].playerX >= -560 && clients[user_id].playerX <= -230)
			return true;
		if (clients[user_id].playerX >= 240 && clients[user_id].playerX <= 560)
			return true;
	}
	//right bottom seesaw
	if (clients[user_id].playerX >= 225 && clients[user_id].playerX <= 565) {
		if (clients[user_id].playerZ >= -432 && clients[user_id].playerZ <= -258)
			return true;

		if (clients[user_id].playerZ >= -630 && clients[user_id].playerZ <= -466)
			return true;
	}
	//left bottom horse
	if (clients[user_id].playerX >= -474 && clients[user_id].playerX <= -310) {
		if (clients[user_id].playerZ >= -601 && clients[user_id].playerZ <= -495)
			return true;
		if (clients[user_id].playerZ >= -449 && clients[user_id].playerZ <= -350)
			return true;
		if (clients[user_id].playerZ >= -296 && clients[user_id].playerZ <= -196)
			return true;
	}

	//user
	float Amax_x = clients[user_id].playerX + 27.f;
	float Amin_x = clients[user_id].playerX - 27.f;
	float Amax_z = clients[user_id].playerZ + 27.f;
	float Amin_z = clients[user_id].playerZ - 27.f;

	for (int j = 0; j < MAX_USER; ++j) {
		if (clients[j].m_connected == true) {
			float Bmax_x = clients[j].playerX + 27.f;
			float Bmin_x = clients[j].playerX - 27.f;
			float Bmax_z = clients[j].playerZ + 27.f;
			float Bmin_z = clients[j].playerZ - 27.f;

			if (user_id == j) continue;
			if (Amax_x < Bmin_x || Amin_x > Bmax_x) continue;
			if (Amax_z< Bmin_z || Amin_z > Bmax_z) continue;

			return true;
		}
	}

	return false;
}

int player_hit(int user_id) {

	if (clients[user_id].state == 1) {
		//user
		float Amax_x = clients[user_id].playerX + 37.f;
		float Amin_x = clients[user_id].playerX - 37.f;
		float Amax_z = clients[user_id].playerZ + 37.f;
		float Amin_z = clients[user_id].playerZ - 37.f;

		for (int j = 0; j < MAX_USER; ++j) {
			if (clients[j].m_connected == true) {
				float Bmax_x = clients[j].playerX + 27.f;
				float Bmin_x = clients[j].playerX - 27.f;
				float Bmax_z = clients[j].playerZ + 27.f;
				float Bmin_z = clients[j].playerZ - 27.f;

				if (user_id == j) continue;
				if (Amax_x < Bmin_x || Amin_x > Bmax_x) continue;
				if (Amax_z< Bmin_z || Amin_z > Bmax_z) continue;

				return j;
			}
		}
	}

	return 0;
}

void logic() {
	GameTimer.Reset();
	while (true) {
		GameTimer.Tick(60.f);

		//collision check
		//for (int obj = 0; obj < 7; ++obj) {
		//	for (int i = 0; i < MAX_USER; ++i) {
		//		if (clients[i].m_connected == true) {
		//			g_Object[obj].CheckCollBox(clients[i]);
		//		}
		//	}
		//}

		for (int i = 0; i < MAX_USER; ++i) {
			if (clients[i].m_connected == true) {
				float movement_x = clients[i].velocityX * GameTimer.DeltaTime();
				float movement_y = clients[i].velocityY * GameTimer.DeltaTime();
				float movement_z = clients[i].velocityZ * GameTimer.DeltaTime();
				
				clients[i].playerX += movement_x;
				clients[i].playerY += movement_y;
				clients[i].playerZ += movement_z;


				if (object_collision(i) == true) {	//충돌된상태
					clients[i].playerX -= movement_x * 1.5f;
					clients[i].playerY -= movement_y * 1.5f;
					clients[i].playerZ -= movement_z * 1.5f;
				}

				if (player_hit(i) != 0) {
					clients[player_hit(i)].m_ani_index = 3;
				}

				//printf("%f, %f, %f \n", clients[i].playerX, clients[i].playerY, clients[i].playerZ);
				//printf("%d\n", object_collision(i));
				mapcollision(i);
			}
		}

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

int main()
{
	g_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	
	//initialize clients
	init_Object();
	for (int i = 0; i < MAX_USER; ++i) {
		clients[i].m_connected = false;
	}

	WSADATA WSAData;
	if (WSAStartup(MAKEWORD(2, 2), &WSAData) != 0) {
		cout << "Fail to load WSAStartup\n";
		return 0;
	}

	// Accept =============
	SOCKET listen_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
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
	AcceptEx(listen_socket, client_socket, accept_over.messageBuffer, NULL, sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16, NULL, &accept_over.overlapped);


	thread logic_thread{ logic };

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
			srand((unsigned)time(NULL));
			clients[user_id].playerX = 0;
			clients[user_id].playerZ = 0;
			clients[user_id].playerZ = 0;
			DWORD flags = 0;

			//로그인 정보 수신
			WSARecv(client_socket, &clients[user_id].m_recv_over.wsabuf, 1, NULL, &flags, &clients[user_id].m_recv_over.overlapped, NULL);

			//접속 완료후 accept 대기
			client_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
			ZeroMemory(&accept_over.overlapped, sizeof(accept_over.overlapped));
			AcceptEx(listen_socket, client_socket, accept_over.messageBuffer, NULL, sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16, NULL, &accept_over.overlapped);
		}
			break;
		default:
			break;
		}
	}

	logic_thread.join();

	return 0;
}