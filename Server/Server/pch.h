#pragma once

#include <iostream>
#include<thread>
#include<chrono>

using namespace std;

#include<WS2tcpip.h>
#include<MSWSock.h>
#include<Windows.h>

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "mswsock.lib")

struct Point3D {
	float x, y, z;
};