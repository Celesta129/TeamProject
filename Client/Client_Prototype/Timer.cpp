#include "stdafx.h"
#include "Timer.h"
#include <mmsystem.h>
#pragma comment (lib, "winmm.lib")

CTimer::CTimer()
	:m_SecondsPerCount(0.0), m_DeltaTime(-1.0), m_BaseTime(0),
	m_PausedTime(0), m_PrevTime(0), m_CurrTime(0), m_bStopped(false),
	m_nCurrentFrameRate(0), m_FPSTimeElapsed(0), m_nFramePerSecond(0)
{
	__int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	m_SecondsPerCount = 1.0 / (double)countsPerSec;
}


CTimer::~CTimer()
{
}

void CTimer::Tick(float fLockFPS)
{
	if (m_bStopped)
	{
		m_DeltaTime = 0.0;
		return;
	}

	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	m_CurrTime = currTime;

	// Time difference between this frame and the previous.
	m_DeltaTime = (m_CurrTime - m_PrevTime)*m_SecondsPerCount;

	// Prepare for next frame.
	m_PrevTime = m_CurrTime;


	m_nFramePerSecond++;
	m_FPSTimeElapsed += m_DeltaTime;
	if (m_FPSTimeElapsed >= 1.0f)
	{
		m_nCurrentFrameRate = m_nFramePerSecond;
		m_nFramePerSecond = 0;
		m_FPSTimeElapsed = 0.0f;
	}

	// Force nonnegative.  The DXSDK's CDXUTTimer mentions that if the 
	// processor goes into a power save mode or we get shuffled to another
	// processor, then mDeltaTime can be negative.
	if (m_DeltaTime < 0.0)
	{
		m_DeltaTime = 0.0;
	}
}

void CTimer::Start(void)
{
	__int64 startTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

	if (m_bStopped)
	{
		m_PausedTime += (startTime - m_StopTime);

		m_PrevTime = startTime;
		m_StopTime = 0;
		m_bStopped = false;
	}
}

void CTimer::Stop(void)
{
	m_bStopped = true;
}

float CTimer::TotalTime() const
{
	if (m_bStopped)
	{
		return (float)(((m_StopTime - m_PausedTime) - m_BaseTime)*m_SecondsPerCount);
	}
	else
	{
		return (float)(((m_CurrTime - m_PausedTime) - m_BaseTime)*m_SecondsPerCount);
	}
}

unsigned long CTimer::GetFrameRate(wstring* pString)
{
	if (pString)
	{
		wstring temp(*pString);
		char FPSBuffer[10];
	
		_itoa_s(m_nCurrentFrameRate, FPSBuffer, 10);
		wstring w_fps(FPSBuffer, &FPSBuffer[10]);

		temp += L": ";
		temp += w_fps.c_str();
		temp += L" FPS";
		*pString = temp;
	}
	return(m_nCurrentFrameRate);
}

float CTimer::GetTimeElapsed() const
{
	return (float)m_DeltaTime;;
}


void CTimer::Reset()
{
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	m_BaseTime = currTime;
	m_PrevTime = currTime;
	m_StopTime = 0;
	m_bStopped = false;
}
