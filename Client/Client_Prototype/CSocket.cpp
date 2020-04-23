#include "stdafx.h"
#include "CSocket.h"

CSocket::CSocket()
{
	memset(buf, 0, MAX_PACKET_SIZE);
}


CSocket::CSocket(char* id, char* ip)
{
	m_pid = id;
	m_pip = ip;
	memset(buf, 0, MAX_PACKET_SIZE);
}

CSocket::~CSocket()
{
	closesocket(clientSocket);
}


void CSocket::DataCleanup()
{
	if (clientSocket) closesocket(clientSocket);
	WSACleanup();
}

void CSocket::init()
{
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		cout << "Error - Invalid socket\n";
	}

	clientSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (clientSocket == INVALID_SOCKET)
	{
		cout << "Error - Invalid socket\n";
	}

	int option = TRUE;
	setsockopt(clientSocket, IPPROTO_TCP, TCP_NODELAY, (const char*)&option, sizeof(option));

	ZeroMemory(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	//serveraddr.sin_addr.s_addr = inet_addr(serverip);
	//serverAddr.sin_addr.s_addr = inet_addr("218.37.39.194");
	//char addr[30];
	//cout << "ipÁÖ¼Ò:";
	//cin >> addr;
	serverAddr.sin_addr.s_addr = inet_addr(m_pip);
	serverAddr.sin_port = htons(SERVER_PORT);
	if (connect(clientSocket, (SOCKADDR *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		cout << "Error - Invalid socket\n";
	}
}

void CSocket::sendPacket(char type, char key, char state, char id)
{
	switch (type)
	{
	case CS_LOGIN: {
		cs_packet_connect p_connect;
		p_connect.type = CS_LOGIN;
		p_connect.size = sizeof(cs_packet_connect);
		int t_id = GetCurrentProcessId();
		sprintf_s(p_connect.id, "P%03d", t_id % 1000);
		send(clientSocket, (char*)& p_connect, sizeof(cs_packet_connect), 0);
	}
		break;
	case CS_DISCONNECT:
		cs_packet_disconnect p_disconnect;
		p_disconnect.type = CS_DISCONNECT;
		p_disconnect.size = sizeof(cs_packet_disconnect);
		send(clientSocket, (char*)& p_disconnect, sizeof(cs_packet_disconnect), 0);
		break;
	case CS_MOVE:
		cs_packet_move p_move;
		p_move.type = CS_MOVE;
		p_move.size = sizeof(cs_packet_move);
		p_move.keydown = state;
		p_move.direction = key;
		send(clientSocket, (char*)& p_move, sizeof(cs_packet_move), 0);
		break;
	default:
		cout << "Error in SendPacket, Invalid Key Input or Type \n";
		break;
	}
}
