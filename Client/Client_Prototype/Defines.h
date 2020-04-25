#pragma once
#define FRAME_BUFFER_WIDTH 1280
#define FRAME_BUFFER_HEIGHT 720

#define NEAR_PLANE_DISTANCE 1.0f
#define FAR_PLANE_DISTANCE 1000.f

#define BASE_CAMERA_FOV 0.25f*MathHelper::Pi
#define BASE_CAMERA_ASPECT (float)FRAME_BUFFER_WIDTH / (float)FRAME_BUFFER_HEIGHT


#define MAX_OBJECT 255
#define NUM_FRAME_RESOURCE NUM_FRAME_RESOURCE

const UINT LOOP_IN = 0;
const UINT LOOP_END = 1;
const UINT LOOP_TRIGGER = 2;
const UINT LOOP_STOP = 3;
const UINT LOOP_SKIP = 4;

//캐릭터 애니메이션
#define Anim_Idle					0
#define Anim_Walk					1
#define Anim_Run					2
#define Anim_Jump					3
#define Anim_Guard					4