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
	// ī�޶� ��ȯ���
	XMFLOAT4X4 m_xmf4x4View;
	// ���� ��ȯ ���
	XMFLOAT4X4 m_xmf4x4Projection;

	// ����Ʈ�� �����簢��
	D3D12_VIEWPORT m_d3dViewport;
	D3D12_RECT m_d3dScissorRect;

	// ī�޶��� ��ġ(������ǥ��) �����̴�.
	XMFLOAT3 m_xmf3Position;
	// ī�޶� x, y, z�� �󸶳� ȸ���ߴ°��� ��Ÿ���� ����.
	XMFLOAT3 m_xmf3Right;
	XMFLOAT3 m_xmf3Look;
	XMFLOAT3 m_xmf3Up;

	float m_fPitch;
	float m_fYaw;
	float m_fRoll;

	float m_fTheta = -1.57f;		// ���� -90��
	float m_fPhi = 0.75f;
	float m_fRadius = 500.0f;

	// �÷��̾ �ٶ� ��ġ����. 3��Ī���� ���
	XMFLOAT3 m_xmf3LookAtWorld;
	// �÷��̾�� ī�޶��� ������. 3��Ī ���
	XMFLOAT3 m_xmf3Offset;

	DWORD m_nMode;
	float m_fTimeLag;

	CGameObject* m_pTarget = NULL;
public:
	CCamera();
	CCamera(CCamera* pCamera);
	virtual ~CCamera();
public:
	virtual void Update(float fTimeElapsed);

	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);

	void GenerateViewMatrix(XMVECTOR pos, XMVECTOR target, XMVECTOR up);
	void GenerateViewMatrix(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3LookAt, XMFLOAT3 xmf3Up);
	void GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float	fAspectRatio, float fFOVAngle);
	/*ī�޶� ������ ȸ���� �ϰ� �Ǹ� ������ �Ǽ� ������ ����Ȯ�� ������ ī�޶��� ���� x-��(Right), y-��(Up), z- ��(LookAt)�� ���� �������� ���� �� �ִ�.\
	ī�޶��� ���� x-��(Right), y-��(Up), z-��(LookAt)�� ���� �����ϵ��� ������ش�.*/
	void RegenerateViewMatrix();

public:
	void SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight, float fMinZ = 0.0f, float fMaxZ = 1.0f);
	void SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom);
	virtual void SetViewportsAndScissorRects(ID3D12GraphicsCommandList *pd3dCommandList);

	void SetTarget(CGameObject *pObject) { m_pTarget = pObject; }
	void SetMode(DWORD nMode) { m_nMode = nMode; }

	void SetPosition(XMFLOAT3 xmf3Position) { m_xmf3Position = xmf3Position; }
	void SetLookAtPosition(XMFLOAT3 xmf3LookAtWorld) { m_xmf3LookAtWorld = xmf3LookAtWorld; }

	void SetOffset(XMFLOAT3 xmf3Offset) { m_xmf3Offset = xmf3Offset; }
	void SetTimeLag(float fTimeLag) { m_fTimeLag = fTimeLag; }

	void SetTheta(float dTheta) { m_fTheta = dTheta; };
	void SetPhi(float dPhi) { m_fPhi = dPhi; };
	void SetRadius(float dRadius) { m_fRadius = dRadius; };

	void MoveTheta(float dTheta) { m_fTheta += dTheta; }
	void MovePhi(float dPhi) { m_fPhi += dPhi; }
public:
	CGameObject* GetTargetObject() { return(m_pTarget); }
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

	float GetTheta() { return m_fTheta; };
	float GetPhi() { return m_fPhi; };
	float GetRadius() { return m_fRadius; };


	//ī�޶� xmf3Shift ��ŭ �̵��Ѵ�. 
	virtual void Move(const XMFLOAT3& xmf3Shift) { m_xmf3Position.x += xmf3Shift.x;	m_xmf3Position.y += xmf3Shift.y; m_xmf3Position.z += xmf3Shift.z; }
	//ī�޶� x-��, y-��, z-������ ȸ���ϴ� �����Լ��̴�. 
	virtual void Rotate(float fRoll = 0.0f, float fPitch = 0.0f, float  fYaw = 0.0f) { }
	//ī�޶��� �̵�, ȸ���� ���� ī�޶��� ������ �����ϴ� �����Լ��̴�. 
	virtual void Update(XMFLOAT3& xmf3LookAt, float fTimeElapsed) { }
	//3��Ī ī�޶󿡼� ī�޶� �ٶ󺸴� ������ �����Ѵ�. �Ϲ������� �÷��̾ �ٶ󺸵��� �����Ѵ�. 
	virtual void SetLookAt(const XMFLOAT3& xmf3LookAt) { }
};
