#pragma once
class CPlayer
{
public:
	CPlayer();
	~CPlayer();

	void SetPos(float x, float y, float z);
	void GetPos(float * x, float * y, float * z);
	void SetVelocity(float x, float y, float z);
	void GetVelocity(float * x, float * y, float * z);
	
	void SetAnimation_index(int animation_index);
	void GetAnimation_index(int *animation_index);

private:
	float m_posX, m_posY, m_posZ;
	float m_velocityX, m_velocityY, m_velocityZ;
	int m_animation_index = 0;
};

