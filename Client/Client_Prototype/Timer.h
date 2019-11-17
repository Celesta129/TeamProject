#pragma once
const ULONG MAX_SAMPLE_COUNT = 50;
class CTimer
{
private:
	double m_SecondsPerCount;
	double m_DeltaTime;

	__int64 m_BaseTime;
	__int64 m_PausedTime;
	__int64 m_StopTime;
	__int64 m_PrevTime;
	__int64 m_CurrTime;

	bool m_bStopped;

	unsigned long m_nCurrentFrameRate;
	unsigned long m_nFramePerSecond;
	float m_FPSTimeElapsed;
public:
	CTimer();
	virtual ~CTimer();

	void Reset();
	void Tick(float fLockFPS = 0.0f);		// 타이머 시간 갱신
	void Start(void);
	void Stop(void);

	float TotalTime()const; // in seconds

	unsigned long GetFrameRate(wstring* pString = NULL);	// 프레임레이트 반환
	float GetTimeElapsed() const;			// 프레임의 평균 경과시간 반환

};

