#include "stdafx.h"
#include "Camera.h"


CCamera::CCamera()
{
}

CCamera::CCamera(CCamera * pCamera)
{
}


CCamera::~CCamera()
{
}

void CCamera::Update(float fTimeElapsed)
{

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
}

void CCamera::GenerateViewMatrix(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3LookAt, XMFLOAT3 xmf3Up)
{
}

void CCamera::GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fAspectRatio, float fFOVAngle)
{
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
