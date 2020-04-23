#pragma once
#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "protocol.h"

class CSocket
{
public:
	WSADATA wsa;
	SOCKET clientSocket;
	SOCKADDR_IN serverAddr;
	char buf[MAX_PACKET_SIZE];
	int packet_size = 0;
	int saved_size = 0;

	char *m_pid, *m_pip;

	CSocket();
	CSocket(char *id, char *ip);
	~CSocket();

	void DataCleanup();

	void init();
	void sendPacket(char type, char key, char state, char id);
};

