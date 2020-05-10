#pragma once

enum ObjectType
{
	Bench, SeeSaw, HobbyHorse, SandBox
};

class CObject
{
protected:
	float m_posX, m_posY, m_posZ;
	float m_sizeX, m_sizeY, m_sizeZ;
	float m_radius;

	int m_Objecttype;

public:
	CObject();
	~CObject();

	void SetObject(float posX, float posY, float posZ, int type);

	//bool CheckCollSphere(CPlayer& trg);

	//bool CheckCollBox(CPlayer& trg);
};

