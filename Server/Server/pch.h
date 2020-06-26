#pragma once

#include <iostream>
#include<thread>
#include<mutex>
#include<chrono>
#include<vector>
#include<queue>
#include "gmtl/gmtl.h"

using namespace std;
using namespace chrono;

#include<WS2tcpip.h>
#include<MSWSock.h>
#include<Windows.h>

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib, "mswsock.lib")
