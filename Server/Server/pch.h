#pragma once

#include <iostream>
#include<thread>
#include<mutex>
#include<chrono>
#include<vector>
#include "gmtl/gmtl.h"

using namespace std;

#include<WS2tcpip.h>
#include<MSWSock.h>
#include<Windows.h>

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "mswsock.lib")
