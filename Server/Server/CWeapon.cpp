#include "CWeapon.h"



CWeapon::CWeapon()
{
	owner = -1;
	drop = true;
	weapon_index = -1;
	pos = gmtl::Point3f(1000.f, 1000.f, 1000.f);
}


CWeapon::~CWeapon()
{
}

void CWeapon::init() {
	owner = -1;
	drop = true;
	weapon_index = -1;
	pos = gmtl::Point3f(1000.f, 1000.f, 1000.f);
}