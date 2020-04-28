// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 특정 포함 파일이 들어 있는
// 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
#define _WINSOCK_DEPRECATED_NO_WARNINGS
// Windows 헤더 파일
#include <windows.h>

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <iostream>

#include <WS2tcpip.h>
#include <thread>

#pragma comment(lib, "WS2_32.lib")

// 여기서 프로그램에 필요한 추가 헤더를 참조합니다.
#include <string>

#include <vector>
#include <list>
#include <map>
#include <algorithm>


#include <wrl.h>
#include <shellapi.h>

#include <d3d12.h>
#include "d3dx12.h"
#include <dxgi1_4.h>
#include <d3dcompiler.h>

#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>

using namespace std;
using namespace DirectX;
using namespace DirectX::PackedVector;

using Microsoft::WRL::ComPtr;

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

const int NUM_FRAME_RESOURCE = 3;		// for d3dUtil:: extern const int NUM_FRAMERESOURCE
#include "../Common/d3dUtil.h"

#include "assimp/Importer.hpp"
#include "assimp\cimport.h"
#include "assimp\postprocess.h"
#include "assimp\scene.h"

#pragma comment(lib, "../lib/assimp-vc140-mt.lib")



#include "Extern.h"
#include "Defines.h"
#include "Typedef.h"
#include "Struct.h"
#include "Enum.h"
#include "Macros.h"
#include "Function.h"
#include "Functor.h"
