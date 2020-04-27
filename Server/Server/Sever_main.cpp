#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "pch.h"
#include "protocol.h"


//global
#define MAX_BUFFER 1024

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

	char playerX, playerY, playerZ;
	char player_id[MAX_NAME_LEN];
	

	SOCKETINFO() {
		m_recv_over.wsabuf.len = MAX_BUFFER;
		m_recv_over.wsabuf.buf = m_recv_over.messageBuffer;
		ZeroMemory(&m_recv_over.overlapped, sizeof(WSAOVERLAPPED));
		m_prev_size = 0;
		m_connected = false;
	}
};

SOCKETINFO clients[MAX_USER];
int g_curr_user_id = 0;
HANDLE g_iocp;

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

//임시로 login_ok로 보냄
void send_move_packet(int user_id, int mover) {
	sc_packet_login_ok p;
	p.id = mover;
	p.size = sizeof(p);
	p.type = SC_MOVE;
	p.posX = clients[mover].playerX;
	p.posY = clients[mover].playerY;
	p.posZ = clients[mover].playerZ;

	//통째로 보내면 메모리에 안좋으므로 &붙이자
	send_packet(user_id, &p);
}

//이동 -> 수정필요
void do_move(int user_id, int direction) {
	int x = clients[user_id].playerX;
	int y = clients[user_id].playerY;
	int z = clients[user_id].playerZ;
	switch (direction)
	{
	case CS_UP:
		if (y > 0)	y-= 50;
		break;
	case CS_DOWN:
		if (y < 1500) y += 50;
		break;
	case CS_LEFT:
		if (x > 0) x-= 50;
		break;
	case CS_RIGHT:
		if (x < 1500) x+= 50;
		break;
	default:
		cout << "UnKnown Direction from Client move packet!\n";
		DebugBreak;
		exit(-1);
		break;
	}

	clients[user_id].playerX = x;
	clients[user_id].playerY = y;
	clients[user_id].playerZ = z;
	printf("%s(%d) : %f, %f\n", clients[user_id].player_id, user_id, clients[user_id].playerX, clients[user_id].playerY);
	for (auto& cl : clients) {
		if (true == cl.m_connected)
			send_move_packet(cl.m_id, user_id);
	}
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
	case CS_MOVE: {
		cs_packet_move* packet = reinterpret_cast<cs_packet_move*>(buf);
		do_move(user_id, packet->direction);
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


int main()
{
	g_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
	
	//initialize clients
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
			clients[user_id].playerX = rand() % 1500;
			clients[user_id].playerY = rand() % 1500;
			clients[user_id].playerZ = 0;
			DWORD flags = 0;

			//로그인 정보 수신
			WSARecv(client_socket, &clients[user_id].m_recv_over.wsabuf, 1, NULL, &flags, &clients[user_id].m_recv_over.overlapped, NULL);
			printf("\n");

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

	return 0;
}