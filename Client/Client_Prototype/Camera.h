#pragma once

#define ASPECT_RATIO (float(FRAME_BUFFER_WIDTH) / float(FRAME_BUFFER_HEIGHT))

class CGameObject;

struct VS_CB_CAMERA_INFO
{
	XMFLOAT4X4 m_xmf4x4View;
	XMFLOAT4X4 m_xmf4x4Projection;
};

class CCamera
{
protected:
	// 카메라 변환행렬
	XMFLOAT4X4 m_xmf4x4View;
	// 투영 변환 행렬
	XMFLOAT4X4 m_xmf4x4Projection;

	// 뷰포트와 씨저사각형
	D3D12_VIEWPORT m_d3dViewport;
	D3D12_RECT m_d3dScissorRect;

	// 카메라의 위치(월드좌표계) 벡터이다.
	XMFLOAT3 m_xmf3Position;
	// 카메라가 x, y, z로 얼마나 회전했는가를 나타내는 벡터.
	XMFLOAT3 m_xmf3Right;
	XMFLOAT3 m_xmf3Look;
	XMFLOAT3 m_xmf3Up;

	float m_fPitch;
	float m_fYaw;
	float m_fRoll;

	float m_fTheta = 1.5f*XM_PI;
	float m_fPhi = 0.2f*XM_PI;
	float m_fRadius = 15.0f;

	// 플레이어가 바라볼 위치벡터. 3인칭에서 사용
	XMFLOAT3 m_xmf3LookAtWorld;
	// 플레이어와 카메라의 오프셋. 3인칭 사용
	XMFLOAT3 m_xmf3Offset;

	DWORD m_nMode;
	float m_fTimeLag;

	CGameObject* m_pObject = NULL;
public:
	CCamera();
	CCamera(CCamera* pCamera);
	virtual ~CCamera();
public:
	virtual void Update(float fTimeElapsed);

	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);

	void GenerateViewMatrix();
	void GenerateViewMatrix(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3LookAt, XMFLOAT3 xmf3Up);
	void GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float	fAspectRatio, float fFOVAngle);
	/*카메라가 여러번 회전을 하게 되면 누적된 실수 연산의 부정확성 때문에 카메라의 로컬 x-축(Right), y-축(Up), z- 축(LookAt)이 서로 직교하지 않을 수 있다.\
	카메라의 로컬 x-축(Right), y-축(Up), z-축(LookAt)이 서로 직교하도록 만들어준다.*/
	void RegenerateViewMatrix();

public:
	void SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight, float fMinZ = 0.0f, float fMaxZ = 1.0f);
	void SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom);
	virtual void SetViewportsAndScissorRects(ID3D12GraphicsCommandList *pd3dCommandList);

	void SetObject(CGameObject *pObject) { m_pObject = pObject; }
	void SetMode(DWORD nMode) { m_nMode = nMode; }

	void SetPosition(XMFLOAT3 xmf3Position) { m_xmf3Position = xmf3Position; }
	void SetLookAtPosition(XMFLOAT3 xmf3LookAtWorld) { m_xmf3LookAtWorld = xmf3LookAtWorld; }

	void SetOffset(XMFLOAT3 xmf3Offset) { m_xmf3Offset = xmf3Offset; }
	void SetTimeLag(float fTimeLag) { m_fTimeLag = fTimeLag; }
public:
	CGameObject* GetTargetObject() { return(m_pObject); }
	DWORD GetMode() { return(m_nMode); }

	XMFLOAT3& GetPosition() { return(m_xmf3Position); }
	XMFLOAT3& GetLookAtPosition() { return(m_xmf3LookAtWorld); }

	XMFLOAT3& GetRightVector() { return(m_xmf3Right); }
	XMFLOAT3& GetUpVector() { return(m_xmf3Up); }
	XMFLOAT3& GetLookVector() { return(m_xmf3Look); }

	float& GetPitch() { return(m_fPitch); }
	float& GetRoll() { return(m_fRoll); }
	float& GetYaw() { return(m_fYaw); }


	XMFLOAT3& GetOffset() { return(m_xmf3Offset); }
	float GetTimeLag() { return(m_fTimeLag); }

	XMFLOAT4X4 GetViewMatrix() { return(m_xmf4x4View); }
	XMFLOAT4X4 GetProjectionMatrix() { return(m_xmf4x4Projection); }
	D3D12_VIEWPORT GetViewport() { return(m_d3dViewport); }
	D3D12_RECT GetScissorRect() { return(m_d3dScissorRect); }

	//카메라를 xmf3Shift 만큼 이동한다. 
	virtual void Move(const XMFLOAT3& xmf3Shift) { m_xmf3Position.x += xmf3Shift.x;	m_xmf3Position.y += xmf3Shift.y; m_xmf3Position.z += xmf3Shift.z; }
	//카메라를 x-축, y-축, z-축으로 회전하는 가상함수이다. 
	virtual void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f) { }
	//카메라의 이동, 회전에 따라 카메라의 정보를 갱신하는 가상함수이다. 
	virtual void Update(XMFLOAT3& xmf3LookAt, float fTimeElapsed) { }
	//3인칭 카메라에서 카메라가 바라보는 지점을 설정한다. 일반적으로 플레이어를 바라보도록 설정한다. 
	virtual void SetLookAt(const XMFLOAT3& xmf3LookAt) { }
};
