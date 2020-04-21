#pragma once
class CBase
{
public:
	CBase();
	virtual ~CBase();
public:
	void AddRef() {
		m_nReference++;
	}
	int Release();

private:
	int m_nReference = 0;

};

