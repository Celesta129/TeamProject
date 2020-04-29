#pragma once

#include "Component.h"

// �����ϰ� ��Ŀ���� ������ ��ɵ�� ������ ��Ƴ��� ������Ʈ�Դϴ�.
class CTransform :
	public CComponent
{
public:
	CTransform();
	CTransform(const CTransform& rhs);
	virtual ~CTransform();

	XMFLOAT4X4 Get_World(void) {return m_xmf4x4World;	};
	void Set_World(XMFLOAT4X4* pWorld);
	
	XMFLOAT3 Get_Scale(void);
	XMFLOAT3 Get_Pos(void);

	void Set_Scale(XMFLOAT3* pScale);

	void Rotate(float fPitch, float fYaw, float fRoll);
	void MovePos(const XMFLOAT3& Pos);
	void Set_Pos(const XMFLOAT3& Pos);

public:
	virtual CComponent* Clone(void);
private:
	XMFLOAT4X4 m_xmf4x4World = MathHelper::Identity4x4();
};

