#include "stdafx.h"
#include "Camera.h"


CCamera::CCamera()
{
}

CCamera::CCamera(CCamera * pCamera)
{
	m_xmf3Position = XMFLOAT3(0.f, 0.f, 0.f);
}


CCamera::~CCamera()
{
}

void CCamera::Update(float fTimeElapsed)
{
	GenerateViewMatrix();
}

void CCamera::CreateShaderVariables(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
}

void CCamera::ReleaseShaderVariables()
{
}

void CCamera::UpdateShaderVariables(ID3D12GraphicsCommandList * pd3dCommandList)
{
}

void CCamera::GenerateViewMatrix()
{
	
	XMVECTOR pos = XMVectorSet(m_xmf3Position.x, m_xmf3Position.y, m_xmf3Position.z, 1.f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);;
	
	XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&m_xmf4x4View, view);
}

void CCamera::GenerateViewMatrix(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3LookAt, XMFLOAT3 xmf3Up)
{
}

void CCamera::GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fAspectRatio, float fFOVAngle)
{
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*MathHelper::Pi, fAspectRatio, 1.0f, 1000.0f);
	XMStoreFloat4x4(&m_xmf4x4Projection, P);
}

void CCamera::RegenerateViewMatrix()
{
}

void CCamera::SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight, float fMinZ, float fMaxZ)
{
}

void CCamera::SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom)
{
}

void CCamera::SetViewportsAndScissorRects(ID3D12GraphicsCommandList * pd3dCommandList)
{
}
