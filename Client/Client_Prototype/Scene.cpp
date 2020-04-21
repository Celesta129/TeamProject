#include "stdafx.h"
#include "Scene.h"
#include "D3DApp.h"
#include "Camera.h"
#include "Mesh.h"
#include "../Common/GeometryGenerator.h"
#include "Shader.h"
#include "Camera.h"
#include "ModelObject.h"

CScene::CScene(ComPtr<ID3D12Device> pDevice, ComPtr<ID3D12GraphicsCommandList> pCommandList)
	:m_d3dDevice(pDevice), m_GraphicsCommandList(pCommandList)
{
}


CScene::~CScene()
{
	ReleaseScene();

}

HRESULT CScene::Initialize()
{
	m_pComponent_Manager = CComponent_Manager::GetInstance();
	
	BuildComponents();
	//BuildObject();
	BuildShaders();
	BuildCamera();

	return true;
}

void CScene::BuildShaders()
{
	CShader* pShader = nullptr;
	CGameObject* pObject = nullptr;

	pShader = new CShader();
	pShader->Initialize(m_d3dDevice.Get(),m_GraphicsCommandList.Get());
	m_vShaders.push_back(pShader);

	
	pObject = new CModelObject;
	((CModelObject*)pObject)->Initialize(L"Component_Model_idle_Anim", m_d3dDevice.Get(), m_GraphicsCommandList.Get());
	m_vObjects.push_back(pObject);		// 전체 오브젝트 관리 벡터에 넣는다.
	pShader->Push_Object(pObject);		// 개별 셰이더에도 넣는다.


}

bool CScene::OnKeyboardInput(const float & fTimeElapsed)
{
	return false;
}

bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return false;
}

bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return false;
}

void CScene::BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{

}

void CScene::ReleaseObjects()
{
	
}

void CScene::ResetCmdList(ID3D12GraphicsCommandList * pd3dCommandList)
{
	for (auto& shader : m_vShaders)
	{
		shader->ResetCmd(pd3dCommandList);
	}
}

void CScene::AnimateObjects(float fTimeElapsed)
{
	for (auto shader : m_vShaders)
	{
		// 
	}
}

void CScene::UpdateCamera(const float & fTimeElapsed)
{
	m_pCurrentCamera->Update(fTimeElapsed);

	CGameObject* pTarget = nullptr;
	if (!m_vObjects.empty())
		pTarget = m_vObjects[0];
	if (pTarget == nullptr)
		return;

	CTransform* pTransform = (CTransform*)pTarget->Get_Component(L"Component_Transform");
	if (pTransform == nullptr) {
		_MSG_BOX("CScene::UpdateCamera - 타겟이 Transform Component가 없습니다.");
		return;
	}

	XMFLOAT3 TargetPos = pTransform->Get_Pos();

	// Convert Spherical to Cartesian coordinates.
	mEyePos.x = mRadius * sinf(mPhi)*cosf(mTheta) + TargetPos.x;
	mEyePos.y = mRadius * cosf(mPhi) + TargetPos.y;
	mEyePos.z = mRadius * sinf(mPhi)*sinf(mTheta) + TargetPos.z;

	//pRenderItem->m_Transform.Get_Pos()
	//XMVECTOR targetPosVector = XMVectorSet(mOpaqueRitems[0]->World.m[3][0], mOpaqueRitems[0]->World.m[3][1], mOpaqueRitems[0]->World.m[3][2], 1.f);
	// Build the view matrix.
	XMVECTOR pos = XMVectorSet(mEyePos.x, mEyePos.y, mEyePos.z, 1.0f);
	XMVECTOR target = XMVectorZero(); //targetPosVector
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, view);
}

void CScene::Update(const CTimer& timer, ID3D12Fence* pFence, ID3D12GraphicsCommandList * cmdList)
{
	OnKeyboardInput(timer.DeltaTime());
	UpdateCamera(timer.DeltaTime());


	for (auto& object : m_vObjects)
	{
		object->Update(timer.DeltaTime());
	}
	for (auto& shader : m_vShaders)
	{
		shader->Update(timer,  pFence, cmdList,m_pCurrentCamera);
	}
	
}

void CScene::Render(ID3D12GraphicsCommandList * cmdList, UINT64 nFenceValue)
{

	for (auto shader : m_vShaders)
	{
		shader->Render(cmdList, nullptr, nFenceValue);
	}
}

void CScene::ReleaseUploadBuffers()
{
}

void CScene::OnResize(float fAspectRatio)
{
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f*MathHelper::Pi, fAspectRatio, 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProj, P);

	m_pCurrentCamera->Update(0);
}

void CScene::ReleaseScene(void)
{
	ReleaseShaders();
	ReleaseCameras();

}

void CScene::ReleaseShaders(void)
{
	for (auto shader : m_vShaders)
	{
		shader->Release();
	}
	m_vShaders.clear();
}

void CScene::ReleaseCameras(void)
{
	for (auto camera : m_vCameras)
	{
		if (camera)
			delete camera;
	}

	if (m_pCurrentCamera)
		delete m_pCurrentCamera;

}

void CScene::BuildComponents(void)
{
	CComponent* pComponent = nullptr;

	pComponent = new CTransform;
	CComponent_Manager::GetInstance()->Add_Component(L"Component_Transform", pComponent);

	pComponent = new RenderItem;
	CComponent_Manager::GetInstance()->Add_Component(L"Component_RenderItem", pComponent);

	pComponent = new LoadModel("resources/idle_Anim.FBX");
	CComponent_Manager::GetInstance()->Add_Component(L"Component_Model_idle_Anim", pComponent);
}

void CScene::BuildRootSignature(void)
{
}

void CScene::BuildShadersAndInputLayout(void)
{
	
}


void CScene::BuildPSOs(void)
{
	
}

void CScene::BuildFrameResources(void)
{
}

void CScene::BuildShapeGeometry(void)
{
	
}

void CScene::BuildRenderItems(void)
{
	
}

void CScene::BuildMesh(void)
{
}

void CScene::BuildCamera(void)
{
	m_pCurrentCamera = new CCamera();
	if (m_vObjects.empty())
		return;

	if (m_vObjects[0] == nullptr)
		return;

	if (m_vObjects[0]->Get_Component(L"Component_Transform") == nullptr)
		return;


	m_pCurrentCamera->SetTarget(m_vObjects[0]);
}

void CScene::BuildObject(void)
{

}
