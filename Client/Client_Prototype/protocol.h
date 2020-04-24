#pragma once

constexpr int MAX_PACKET_SIZE = 1024;

constexpr int SERVER_PORT = 9000;
constexpr int MAX_USER = 8;

//Client to Sever type
constexpr int CS_LOGIN = 1;
constexpr int CS_DISCONNECT = 2;
constexpr int CS_MOVE = 3;

constexpr int CS_UP = 10;
constexpr int CS_DOWN = 11;
constexpr int CS_LEFT = 12;
constexpr int CS_RIGHT = 13;

constexpr int CS_ATTACK = 14;
constexpr int CS_GUARD = 15;
constexpr int CS_DASH = 16;


//Server to clients type
constexpr int SC_CONNECTED = 1;
constexpr int SC_ENTER = 2;
constexpr int SC_LEAVE = 3;


constexpr int SC_MOVEMENT = 11;


#pragma pack(push ,1)
struct cs_packet_connect {
	char size;
	char type;
	char id[15];
};

struct cs_packet_disconnect {
	char size;
	char type;
};


struct cs_packet_move {
	char size;
	char type;
	char direction;
	char keydown;
};

struct cs_packet_motion{
	char size;
	char type;
	char key;
	char count;
};

struct sc_packet_login_ok {
	char size;
	char type;
	int id;
	float posX, posY, posZ;
};

struct sc_packet_enter {
	char size;
	char type;
	int id;
	int posX, posY, posZ;
};

struct sc_packet_leave {
	char size;
	char type;
	int id;
};

struct sc_packet_move {
	char size;
	char type;
	char id;
	float x, y, z;
	float vx, vy, vz;
	char ani_index;
	char dashed;
};

#pragma pack(pop)