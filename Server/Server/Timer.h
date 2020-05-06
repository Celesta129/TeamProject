#pragma once
const ULONG MAX_SAMPLE_COUNT = 50;
class CTimer
{
public:
	CTimer();
	virtual ~CTimer();

	void Reset(void);
	void Tick(float fFPS);		// 타이머 시간 갱신
	void Start(void);
	void Stop(void);

	float TotalTime(void) const; // in seconds
	float DeltaTime(void) const;
private:
	double m_SecondsPerCount;
	double m_DeltaTime;

	__int64 m_BaseTime;
	__int64 m_PausedTime;
	__int64 m_StopTime;
	__int64 m_PrevTime;
	__int64 m_CurrTime;

	bool m_bStopped;

};

