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
public:
	CTimer();
	virtual ~CTimer();

	void Tick(float fLockFPS = 0.0f);		// Ÿ�̸� �ð� ����
	void Start(void);
	void Stop(void);

	unsigned long GetFrameRate(LPTSTR lpszString = NULL, int nCharacters = 0);	// �����ӷ���Ʈ ��ȯ
	float GetTimeElapsed() const;			// �������� ��� ����ð� ��ȯ

	void Reset();
};

