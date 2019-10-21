#pragma once
const ULONG MAX_SAMPLE_COUNT = 50;
class CTimer
{
private:
	bool m_bHardwareHasPerformanceCounter;
	float m_fTimeScale;
	float m_fTimeElapsed;

	__int64 m_nCurrentTime;
	__int64 m_nLastTime;
	__int64 m_nPerformanceFrequency;

	float m_fFrameTime[MAX_SAMPLE_COUNT];
	ULONG m_nSampleCount;

	unsigned long m_nCurrentFrameRate;
	unsigned long m_nFramePerSecond;
	float m_fFPSTimeElapsed;
public:
	CTimer();
	virtual ~CTimer();

	void Tick(float fLockFPS = 0.0f);		// Ÿ�̸� �ð� ����
	unsigned long GetFrameRate(LPTSTR lpszString = NULL, int nCharacters = 0);	// �����ӷ���Ʈ ��ȯ
	float GetTimeElapsed();			// �������� ��� ����ð� ��ȯ

	void Reset();
};

