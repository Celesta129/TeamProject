#pragma once
#include "gmtl/gmtl.h"

struct Weapon_respawn {
	char type = 0;
	char index = 0;
	bool respawn = true;
};

struct CFlag {
	bool drop = true;
	char owner = -1;
	gmtl::Point3f pos = gmtl::Point3f(0.f, 0.f, 0.f);
};

class CWeapon
{
public:
	char owner;
	char weapon_index;
	bool drop;
	gmtl::Point3f pos;

	CWeapon();
	~CWeapon();

	void init();
};

