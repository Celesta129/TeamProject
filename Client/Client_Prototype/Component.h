#pragma once
#include "stdafx.h"

class CComponent
{
public:
	CComponent();
	virtual ~CComponent();

public:
	void Addref(void) { m_nReference++; }
	UINT release(void);
private:
	UINT m_nReference = 0;
public:
	virtual CComponent* Clone(void) = 0;		// Clone 구현시 new Component(this)와 return this; 구분할것. 즉, 새로 복사-생성해서 넘길것과 자신의 포인터만 넘길 컴포넌트를 구분해야함.
												// 이렇게 구현하려면 정확히는 레퍼런스 카운팅을 해서 Release 함수를 만들어야겠지만 시간상 생략함. 
												// 컴포넌트 매니저로부터 복사받은 컴포넌트를 갖고있는 오브젝트는 자신의 컴포넌트 삭제시 반드시! 포인터만 갖고있는것인지 새로 복사생성한것인지 알수있는 bool변수 체크코드를 넣어야함.
												// 구조적으로 레퍼런스 카운팅이 100만배 낫지만 다시 말하지만 시간상 생략

												// 즉 구현시 1. 매니저가 포인터를 넘겨주는 경우 IsManagers = true, return this;
												// 2. 매니저가 새로 복사생성해서 넘겨주는경우 IsManagers = false, return new(Component); 일것임. 주의 또 주의!
	
};

