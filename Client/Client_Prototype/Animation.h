#pragma once
#include "stdafx.h"
class CAnimation
{
public:
	CAnimation();
	CAnimation(const CAnimation& rhs);
	virtual ~CAnimation();
public:
	aiAnimation* GetAnim(void) {
		return m_pAnimation;
	};
	void SetaiAnim(aiAnimation* pAnim) {
		m_pAnimation = pAnim;
	}
public:
	bool					m_bAnimationLoop = true;
	float					m_fAnimSpeed = 1.f;
	float					m_fStart_time = 0.f; //프레임 시작 시간
	float					m_fEnd_time = 0.f;  //프레임 종료 시간
	float					m_fTrigger_time = 0.f;	//프레임 중간 시간.트리거용도로 사용
	float					m_fNow_time = 0.f;  //현재 프레임
	float					m_fPosible_skip = 0.f; //애니메이션을 강제 종료하고 다음 애니메이션 실행 가능한 프레임

private:
	aiAnimation* m_pAnimation = nullptr;

};

