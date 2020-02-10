#pragma once

#include "Component.h"

// 순수하게 행렬연산과 연관된 기능들과 변수를 모아놓은 컴포넌트입니다.
class CTransform :
	public CComponent
{
public:
	CTransform();
	virtual ~CTransform();

	XMFLOAT4X4 Get_World(void) {return m_xmf4x4World;	};
	void Set_World(XMFLOAT4X4* pWorld);
	
	XMFLOAT3 Get_Scale(void);
	XMFLOAT3 Get_Pos(void);

	void Set_Scale(XMFLOAT3* pScale);

	void MovePos(XMFLOAT3* pPos);
	void Set_Pos(XMFLOAT3* pPos);

public:
	virtual CComponent* Clone(void);
private:
	XMFLOAT4X4 m_xmf4x4World;
};

