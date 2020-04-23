#include "stdafx.h"
#include "Scene.h"
#include "D3DApp.h"
#include "Camera.h"
#include "Mesh.h"
#include "../Common/GeometryGenerator.h"

#include "ObjectShader.h"
#include "AxisShader.h"

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
	CTransform* pTransform = nullptr;

	pShader = new CObjectShader();
	pShader->Initialize(m_d3dDevice.Get(),m_GraphicsCommandList.Get(), L"Shaders\\color.hlsl");
	m_vShaders.push_back(pShader);

	
	pObject = new CModelObject;
	dynamic_cast<CModelObject*>(pObject)->Initialize(L"Component_Model_idle", m_d3dDevice.Get(), m_GraphicsCommandList.Get());
	m_vObjects.push_back(pObject);		// 전체 오브젝트 관리 벡터에 넣는다.
	pShader->Push_Object(pObject);		// 개별 셰이더에도 넣는다.
	pTransform = GET_COMPNENT(CTransform*, m_vObjects[0], L"Component_Transform");
	pTransform->Rotate(90.f, 0.f, 0.f);


	pShader = new CAxisShader;
	pShader->Initialize(m_d3dDevice.Get(), m_GraphicsCommandList.Get(), L"Shaders\\Axis.hlsl");
	m_vShaders.push_back(pShader);

	pObject = new CModelObject;
	((CModelObject*)pObject)->Initialize(L"Component_Model_xyz", m_d3dDevice.Get(), m_GraphicsCommandList.Get());
	m_vObjects.push_back(pObject);		// 전체 오브젝트 관리 벡터에 넣는다.
	pShader->Push_Object(pObject);		// 개별 셰이더에도 넣는다.

	//pObject = new CModelObject;
	//((CModelObject*)pObject)->Initialize(L"Component_Model_idle", m_d3dDevice.Get(), m_GraphicsCommandList.Get());
	//m_vObjects.push_back(pObject);		// 전체 오브젝트 관리 벡터에 넣는다.
	//pShader->Push_Object(pObject);		// 개별 셰이더에도 넣는다.
}

bool CScene::OnKeyboardInput(const float & fTimeElapsed)
{
	float fSpeed = 5.f;
	XMFLOAT3 xmf3Move = XMFLOAT3(0.f,0.f,0.f); 

	if (GetAsyncKeyState('A') & 0x8000)
	{
		xmf3Move = XMFLOAT3(-fSpeed * fTimeElapsed, 0.f, 0.f);
	
		m_pCurrentCamera->MoveTheta(fSpeed*fTimeElapsed);
		//CTransform* pTransform = (CTransform*)m_vObjects[0]->Get_Component(L"Component_Transform");
		//pTransform->MovePos(&xmf3Move);
		//m_vObjects[0]->DirtyFrames();	// 값을 Update에서 갱신해야한다.

	}
	if (GetAsyncKeyState('D') & 0x8000)
	{
		m_pCurrentCamera->MoveTheta(-fSpeed*fTimeElapsed);
		xmf3Move = XMFLOAT3(fSpeed * fTimeElapsed, 0.f, 0.f);
		
	}
	if (GetAsyncKeyState('W') & 0x8000)
	{
		m_pCurrentCamera->MovePhi(fSpeed * fTimeElapsed);
		xmf3Move = XMFLOAT3(0.f, fSpeed * fTimeElapsed, 0.f);
		
	}
	if (GetAsyncKeyState('S') & 0x8000)
	{
		m_pCurrentCamera->MovePhi(-fSpeed * fTimeElapsed);
		xmf3Move = XMFLOAT3(0.f, -fSpeed * fTimeElapsed, 0.f);
	}
	if (GetAsyncKeyState('Q') & 0x8000)
	{
		xmf3Move = XMFLOAT3(0.f, 0.f, -fSpeed * fTimeElapsed);
	}
	if (GetAsyncKeyState('E') & 0x8000)
	{
		xmf3Move = XMFLOAT3(0.f, 0.f, fSpeed * fTimeElapsed);
	}
	if (GetAsyncKeyState('X') & 0x8000)
	{
		xmf3Move = XMFLOAT3(fSpeed * fTimeElapsed, fSpeed * fTimeElapsed, fSpeed * fTimeElapsed);
		if (m_vObjects[1]) {

			CTransform* pTransform = (CTransform*)m_vObjects[1]->Get_Component(L"Component_Transform");
			pTransform->Rotate(xmf3Move.x, xmf3Move.y, xmf3Move.z);
			//pTransform->
			m_vObjects[1]->DirtyFrames();
		}
	}
	
	//m_pCurrentCamera->Move(xmf3Move);

	return false;
}

void CScene::OnMouseDown(WPARAM btnState, int x, int y, const POINT& lastPoint)
{
}

void CScene::OnMouseMove(WPARAM btnState, int x, int y, const POINT& lastPoint)
{
	if (m_pCurrentCamera == nullptr)
		return;

	float Unit = 0.25f;		//0.25f
	float Unit2 = 0.05f;		//0.05f
	float fMax_Radius = 200.f;
	float fMin_Radius = 50.f;


	if ((btnState & MK_LBUTTON) != 0)
	{
		
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(Unit*static_cast<float>(x - lastPoint.x));
		float dy = XMConvertToRadians(Unit*static_cast<float>(y - lastPoint.y));

		// Update angles based on input to orbit camera around box.
		float fTheta = m_pCurrentCamera->GetTheta();
		float fPhi = m_pCurrentCamera->GetPhi();

		fTheta += dx;
		fPhi += dy;

		// Restrict the angle mPhi.
		fPhi = MathHelper::Clamp(fPhi, 0.1f, MathHelper::Pi - 0.1f);

		m_pCurrentCamera->SetTheta(fTheta);
		m_pCurrentCamera->SetPhi(fPhi);

	}
	else if ((btnState & MK_RBUTTON) != 0)
	{
		// Make each pixel correspond to 0.2 unit in the scene.
		float dx = Unit2 *static_cast<float>(x - lastPoint.x);
		float dy = Unit2 *static_cast<float>(y - lastPoint.y);

		float fRadius = m_pCurrentCamera->GetRadius();
		// Update the camera radius based on input.
		fRadius += dx - dy;
	
		// Restrict the radius.
		fRadius = MathHelper::Clamp(fRadius, fMin_Radius, fMax_Radius);

		m_pCurrentCamera->SetRadius(fRadius);
	}

	
}

void CScene::OnMouseUp(WPARAM btnState, int x, int y, const POINT& lastPoint)
{
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
	for (int index = (int)m_vShaders.size() - 1; index >= 0; --index)
	{
		CShader* pShader = m_vShaders[index];
		if (pShader != nullptr)
		{
			// Allocator Reset은 호출시점의 해당 Allocator를 CommandList가 '사용하고 있지 않아야'한다.
			// ID3D12GraphicsCommandList->Reset은
			// 1. Allocator의 Reset과 달리 어느때나 불러도 상관없다.
			// 2. 상태가 Open된다. 동시에 인자로 받은 CommandAllocator에 기록을 시작한다.
			// 해당 Allocator에 기록된 명령들은 안사라진다. 즉, Allocator의 리셋이 동반되어야 우리가 원하는 리셋이 될 것이다.
			// 두번째 인자인 PipelineState가 NULL인 이유에 대해서는 MSDN 참고.

			// 즉 우리가 원하는 셰이더의 순차적 렌더링을 하려면 m_vShaders안의 각 셰이더가 멤버로 갖고있는 프레임 리소스의 CommandAllocator를 셰이더 역순으로 리셋시키고
			// m_vShader의 0번째 인덱스에 있는 Shader, 즉 첫번째 셰이더의 CommandAllocator를 마지막으로 리셋해준다.
			
			pShader->ResetCmd(pd3dCommandList);
			if(index > 0)
				ThrowIfFailed(pd3dCommandList->Close());
		}
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
	if(m_pCurrentCamera)
		m_pCurrentCamera->Update(fTimeElapsed);

}

void CScene::Update(const CTimer& timer, ID3D12Fence* pFence, ID3D12GraphicsCommandList * cmdList)
{
	
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

void CScene::Render(ID3D12GraphicsCommandList * cmdList, UINT64& nFenceValue)
{
	for (auto& shader : m_vShaders)
	{
		shader->Render(cmdList, nullptr, nFenceValue);
	}
}

void CScene::ReleaseUploadBuffers()
{
}

void CScene::OnResize(float fAspectRatio)
{
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
	CComponent_Manager::GetInstance()->Add_Component(L"Component_Model_idle", pComponent);

	pComponent = new LoadModel("resources/xyz.FBX");
	CComponent_Manager::GetInstance()->Add_Component(L"Component_Model_xyz", pComponent);

	pComponent = new LoadModel("resources/x.FBX");
	CComponent_Manager::GetInstance()->Add_Component(L"Component_Model_x", pComponent);

	pComponent = new LoadModel("resources/y.FBX");
	CComponent_Manager::GetInstance()->Add_Component(L"Component_Model_y", pComponent);

	pComponent = new LoadModel("resources/z.FBX");
	CComponent_Manager::GetInstance()->Add_Component(L"Component_Model_z", pComponent);
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
