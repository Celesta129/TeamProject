#include "Animation.h"



CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation & rhs)
{
	m_pAnimation = rhs.m_pAnimation;

	m_bAnimationLoop = rhs.m_bAnimationLoop;
	m_fAnimSpeed = rhs.m_fAnimSpeed;
	m_fEnd_time = rhs.m_fEnd_time;
	m_fNow_time = rhs.m_fNow_time;
	m_fPosible_skip = rhs.m_fPosible_skip;
	m_fStart_time = rhs.m_fStart_time;
	m_fTrigger_time = rhs.m_fTrigger_time;
	
}


CAnimation::~CAnimation()
{
	
}
