#include "stdafx.h"
#include "Camera.h"
#include "GameObject.h"
#include "Transform.h"

CCamera::CCamera()
{
	//m_xmf3Position = XMFLOAT3(0.f, sqrtf(750.f), sqrtf(750.f));
	m_xmf3Position = XMFLOAT3(0.f, sqrtf(5.f), sqrtf(5.f));
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
	// Convert Spherical to Cartesian coordinates.
	m_xmf3Position.x = m_fRadius * sinf(m_fPhi)*cosf(m_fTheta);
	m_xmf3Position.z = m_fRadius * sinf(m_fPhi)*sinf(m_fTheta);
	m_xmf3Position.y = m_fRadius * cosf(m_fPhi);

	if (m_pObject)
	{
		CTransform* pTransfrom = &m_pObject->GetRenderItem()->m_Transform;

		m_xmf3Position.x += pTransfrom->GetPos().x;
		m_xmf3Position.z += pTransfrom->GetPos().z;
		m_xmf3Position.y += pTransfrom->GetPos().y;
	}

	//XMVECTOR targetPosVector = XMVectorSet(mOpaqueRitems[0]->World.m[3][0], mOpaqueRitems[0]->World.m[3][1], mOpaqueRitems[0]->World.m[3][2], 1.f);
	// Build the view matrix.
	XMVECTOR pos = XMVectorSet(m_xmf3Position.x, m_xmf3Position.y, m_xmf3Position.z, 1.0f);
	XMVECTOR target = XMVectorZero(); //targetPosVector
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&m_xmf4x4View, view);

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
	XMMATRIX P = XMMatrixPerspectiveFovLH(fFOVAngle, fAspectRatio, fNearPlaneDistance, fFarPlaneDistance);
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

