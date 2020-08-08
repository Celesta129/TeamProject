#pragma once
#include "def.h"

class CRoom
{
public:
	int room_num;
	char name[MAX_ROOM_NAME_LEN];

	int clientNum[MAX_ROOM_USER];
	char now_user_num;
	char max_num;
	char host_num;
	char room_map;

	char room_state;

	CRoom();
	~CRoom();

	void init();
	void AddPlayer();
	void RemovePlayer();
	void RoomClear();
};

