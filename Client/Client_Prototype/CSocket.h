#pragma once
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "protocol.h"

class CSocket
{
public:
	WSADATA wsa;
	SOCKET clientSocket;
	SOCKADDR_IN serverAddr;
	char buf[MAX_PACKET_SIZE];
	char packetBuffer[MAX_PACKET_SIZE];

	int packet_size = 0;
	int saved_size = 0;

	char *m_pid, *m_pip;

	CSocket();
	CSocket(char *id, char *ip);
	~CSocket();

	void DataCleanup();

	bool init();
	void sendPacket(char type, char key, char state, char id);
};

