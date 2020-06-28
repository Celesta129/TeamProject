#pragma once

#include "Component.h"
// 순수하게 행렬연산과 연관된 기능들과 변수를 모아놓은 컴포넌트입니다.
class CTransform :
	public CComponent
{
public:
	enum INFORMATION {
		INFO_RIGHT, INFO_UP, INFO_LOOK, INFO_POSITION
	};
public:
	CTransform();
	CTransform(const CTransform& rhs);
	virtual ~CTransform();

	XMFLOAT4X4 Get_World(void);

	void Set_Parent(XMFLOAT4X4* pParent);
	void Set_World(XMFLOAT4X4* pWorld);
	
	const XMFLOAT3 Get_Scale(void);
	const XMFLOAT3 Get_Pos(void);
	const XMFLOAT3 Get_Dir(void);
	const XMFLOAT3 Get_Rotate(void);

	void Set_Scale(const XMFLOAT3& pScale);
	void Set_Pos(const XMFLOAT3& Pos);
	void Set_Rotate(float fPitch, float fYaw, float fRoll);
	void Set_Rotate(const XMFLOAT3& rotate);
	void Set_Look(XMFLOAT3& xmf3Look);

	void Rotate(float fPitch, float fYaw, float fRoll);
	void Rotate(const XMFLOAT3& rotate);
	void MovePos(const XMFLOAT3& Pos);
	void Go_Straight(const float fSpeed);

public:
	virtual CComponent* Clone(void);
private:
	float m_fRoll = 0.f, m_fPitch = 0.f, m_fYaw = 0.f;

	XMFLOAT4X4 m_xmf4x4World = MathHelper::Identity4x4();
	XMFLOAT4X4* m_xmf4x4Parent = nullptr;
};

