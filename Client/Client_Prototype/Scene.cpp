#include "stdafx.h"
#include "Scene.h"
#include "D3DApp.h"
#include "Camera.h"
#include "Mesh.h"
#include "../Common/GeometryGenerator.h"
#include "Shader.h"
#include "Camera.h"

CScene::CScene(ComPtr<ID3D12Device> pDevice, ComPtr<ID3D12GraphicsCommandList> pCommandList)
	:m_d3dDevice(pDevice), m_GraphicsCommandList(pCommandList)
{
}


CScene::~CScene()
{
	for (auto shader : m_vShaders)
	{
		shader->Release();
	}
	m_vShaders.clear();

	for (auto camera : m_vCameras)
	{
		if (camera)
			delete camera;
	}

	if (m_pCurrentCamera)
		delete m_pCurrentCamera;


	if (m_pBoxMesh)
		delete m_pBoxMesh;
}

bool CScene::Initialize(UINT CbvSrvUavDescriptorSize)
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


	pObject = new CGameObject();
	pObject->Initialize();
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

	RenderItem* pRenderItem = pTarget->GetRenderItem();
	XMFLOAT4X4 TargetWorld = pRenderItem->World;

	// Convert Spherical to Cartesian coordinates.
	mEyePos.x = mRadius * sinf(mPhi)*cosf(mTheta) + TargetWorld.m[3][0];
	mEyePos.z = mRadius * sinf(mPhi)*sinf(mTheta) + TargetWorld.m[3][2];
	mEyePos.y = mRadius * cosf(mPhi) + TargetWorld.m[3][1];

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

void CScene::Render(ID3D12GraphicsCommandList * cmdList)
{

	for (auto shader : m_vShaders)
	{
		shader->Render(cmdList, nullptr);
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


void CScene::BuildDescriptorHeaps()
{
}

void CScene::BuildConstantBufferViews(UINT CbvSrvUavDescriptorSize)
{
	
}

void CScene::BuildComponents(void)
{
	CComponent* pComponent = nullptr;

	pComponent = new RenderItem;
	CComponent_Manager::GetInstance()->Add_Component(L"RenderItem", pComponent);
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

	if (m_vObjects[0]->Get_Component(L"RenderItem") == nullptr)
		return;


	m_pCurrentCamera->SetTarget(m_vObjects[0]);
}

void CScene::BuildObject(void)
{

}
