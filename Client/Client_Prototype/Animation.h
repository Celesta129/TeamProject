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
	float					m_fStart_time = 0.f; //������ ���� �ð�
	float					m_fEnd_time = 0.f;  //������ ���� �ð�
	float					m_fTrigger_time = 0.f;	//������ �߰� �ð�.Ʈ���ſ뵵�� ���
	float					m_fNow_time = 0.f;  //���� ������
	float					m_fPosible_skip = 0.f; //�ִϸ��̼��� ���� �����ϰ� ���� �ִϸ��̼� ���� ������ ������

private:
	aiAnimation* m_pAnimation = nullptr;

};

