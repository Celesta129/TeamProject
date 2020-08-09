#pragma once

constexpr int MAX_PACKET_SIZE = 1024;

constexpr int SERVER_PORT = 9000;
constexpr int MAX_USER = 8;
constexpr int MAX_NAME_LEN = 15;

//Client to Sever type
constexpr int CS_LOGIN = 1;
constexpr int CS_DISCONNECT = 2;
constexpr int CS_MOVEMENT = 3;
constexpr int CS_ITEM = 4;

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

constexpr int SC_PUT_WEAPON = 4;
constexpr int SC_PICK_WEAPON = 5;
constexpr int SC_REMOVE_WEAPON = 6;
constexpr int SC_UNPICK_WEAPON = 7;
constexpr int SC_UPDATE_STATE = 8;
constexpr int SC_TIMER = 9;
constexpr int SC_FLAG_TIMER = 10;

constexpr int SC_MOVEMENT = 11;
constexpr int SC_ATTACK = 12;
constexpr int SC_GUARD = 13;

constexpr int SC_WIN = 21;
constexpr int SC_LOSE = 22;

constexpr int SC_WAITROOM = 31;
constexpr int SC_START = 32;

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

struct cs_packet_motion {
	char size;
	char type;
	char key;
	char count;
};

struct sc_packet_guard {
	char size;
	char type;
	char id;
	char flag;
	char ani_index;
};

struct cs_packet_weapon {
	char size;
	char type;
	char weapon_type;
	char weapon_index;
};

// Server to Clients

struct sc_packet_login_ok {
	char size;
	char type;
	int id;
	float posX, posY, posZ;
};

struct sc_packet_waitroom {
	char size;
	char type;
	char id;
	char name[15];
};

struct sc_packet_start {
	char size;
	char type;
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

struct sc_packet_motion {
	char size;
	char type;
	char id;
	char ani_index;
};

struct sc_packet_put_weapon {
	char size;
	char type;
	char weapon_type;
	char weapon_index;
	int x, y, z;
};

struct sc_packet_remove_weapon {
	char size;
	char type;
	char weapon_type;
	char weapon_index;
};

struct sc_packet_pick_weapon {
	char size;
	char type;
	char id;
	char weapon_type;
	char weapon_index;
};

struct sc_packet_unpick_weapon {
	char size;
	char type;
	char id;
	char weapon_type;
	char weapon_index;
};

struct sc_packet_update_state {
	char size;
	char type;
	char id;
	char hp;
};

//게임 타이머
struct sc_packet_timer {
	char size;
	char type;
	int timer;
};

struct sc_packet_flag_timer {
	char size;
	char type;
	char id;
	int timer;
};

//종료값
struct sc_packet_win_flag {
	char size;
	char type;
};

struct sc_packet_lose_flag {
	char size;
	char type;
};

#pragma pack(pop);