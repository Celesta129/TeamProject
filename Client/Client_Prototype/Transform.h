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

	XMFLOAT4X4 Get_World(void) {return m_xmf4x4World;	};
	void Set_World(XMFLOAT4X4* pWorld);
	
	const XMFLOAT3 Get_Scale(void);
	const XMFLOAT3 Get_Pos(void);
	const XMFLOAT3 Get_Dir(void);

	void Set_Scale(const XMFLOAT3& pScale);
	void Set_Pos(const XMFLOAT3& Pos);
	void Set_Rotate(float fRoll, float fPitch, float fYaw);
	void Set_Look(XMFLOAT3& xmf3Look);
	void Rotate(float fRoll, float fPitch, float fYaw);
	void MovePos(const XMFLOAT3& Pos);
	void Go_Straight(const float fSpeed);

public:
	virtual CComponent* Clone(void);
private:
	XMFLOAT4X4 m_xmf4x4World = MathHelper::Identity4x4();
};

