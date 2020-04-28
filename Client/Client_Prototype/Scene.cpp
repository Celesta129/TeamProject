#include "Scene.h"

#include "Timer.h"
#include "Camera.h"

#include "ObjectShader.h"
#include "AxisShader.h"
#include "Object_TextureShader.h"

#include "ModelObject.h"
#include "Model_TextureObject.h"

CScene::CScene(ComPtr<ID3D12Device> pDevice, ComPtr<ID3D12GraphicsCommandList> pCommandList)
	:m_d3dDevice(pDevice), m_GraphicsCommandList(pCommandList)
{
	m_pComponent_Manager = CComponent_Manager::GetInstance();
	m_pComponent_Manager->AddRef();

}


CScene::~CScene()
{
}

HRESULT CScene::Initialize()
{
	
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

	// For TestObject
	/*pShader = new CObjectShader();
	pShader->Initialize(m_d3dDevice.Get(),m_GraphicsCommandList.Get(), L"Shaders\\color.hlsl");
	m_vShaders.push_back(pShader);*/

	

	// For Axis
	//pShader = new CAxisShader;
	//pShader->Initialize(m_d3dDevice.Get(), m_GraphicsCommandList.Get(), L"Shaders\\Axis.hlsl");
	//m_vShaders.push_back(pShader);

	//pObject = new CModelObject;
	//dynamic_cast<CModelObject*>(pObject)->Initialize(L"Component_Model_xyz", m_d3dDevice.Get(), m_GraphicsCommandList.Get());
	//m_vObjects.push_back(pObject);		// ��ü ������Ʈ ���� ���Ϳ� �ִ´�.
	//pShader->Push_Object(pObject);		// ���� ���̴����� �ִ´�.

	// For TextureObject
	pShader = new CObject_TextureShader;
	pShader->Initialize(m_d3dDevice.Get(), m_GraphicsCommandList.Get(), L"Shaders\\TexObject.hlsl");
	m_vShaders.push_back(pShader);


	pObject = new CModel_TextureObject;
	dynamic_cast<CModel_TextureObject*>(pObject)->Initialize(L"Component_Model_idle", m_d3dDevice.Get(), m_GraphicsCommandList.Get());
	m_vObjects.push_back(pObject);		// ��ü ������Ʈ ���� ���Ϳ� �ִ´�.
	pShader->Push_Object(pObject);		// ���� ���̴����� �ִ´�.
	pTransform = GET_COMPNENT(CTransform*, pObject, L"Component_Transform");
	//pTransform->Rotate(90.f, 0.f, 0.f);

	dynamic_cast<CObject_TextureShader*>(pShader)->setMat(0, 0);
	/*pObject = new CModel_TextureObject;
	dynamic_cast<CModel_TextureObject*>(pObject)->Initialize(L"Component_Model_xyz", m_d3dDevice.Get(), m_GraphicsCommandList.Get())*/;
	// ��ü ������Ʈ ���� ���Ϳ� �ִ´�.

	//pObject = new CModelObject;
	//((CModelObject*)pObject)->Initialize(L"Component_Model_idle", m_d3dDevice.Get(), m_GraphicsCommandList.Get());
	//m_vObjects.push_back(pObject);		// ��ü ������Ʈ ���� ���Ϳ� �ִ´�.
	//pShader->Push_Object(pObject);		// ���� ���̴����� �ִ´�.

	for (int i = 0; i < MAX_USER; ++i) {
		m_player[i] = new CPlayer();
	}
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
		//m_vObjects[0]->DirtyFrames();	// ���� Update���� �����ؾ��Ѵ�.

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
		xmf3Move = XMFLOAT3(fSpeed * fTimeElapsed, 0.f, 0.f);
		if (m_vObjects[0]) {

			CTransform* pTransform = (CTransform*)m_vObjects[0]->Get_Component(L"Component_Transform");
			pTransform->Rotate(xmf3Move.x, xmf3Move.y, xmf3Move.z);
			//pTransform->
			m_vObjects[0]->DirtyFrames();
		}
	}
	if (GetAsyncKeyState('Y') & 0x8000)
	{
		xmf3Move = XMFLOAT3(0.f, fSpeed * fTimeElapsed, 0.f);
		if (m_vObjects[0]) {

			CTransform* pTransform = (CTransform*)m_vObjects[0]->Get_Component(L"Component_Transform");
			pTransform->Rotate(xmf3Move.x, xmf3Move.y, xmf3Move.z);
			//pTransform->
			m_vObjects[0]->DirtyFrames();
		}
	}
	if (GetAsyncKeyState('Z') & 0x8000)
	{
		xmf3Move = XMFLOAT3(0.f, 0.f, fSpeed * fTimeElapsed);
		if (m_vObjects[0]) {

			CTransform* pTransform = (CTransform*)m_vObjects[0]->Get_Component(L"Component_Transform");
			pTransform->Rotate(xmf3Move.x, xmf3Move.y, xmf3Move.z);
			//pTransform->
			m_vObjects[0]->DirtyFrames();
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
	for (auto& pObject : m_vObjects) 
	{
		int refCnt = Safe_Release(pObject);
	}
}

void CScene::ResetCmdList(ID3D12GraphicsCommandList * pd3dCommandList)
{
	for (int index = (int)m_vShaders.size() - 1; index >= 0; --index)
	{
		CShader* pShader = m_vShaders[index];
		if (pShader != nullptr)
		{
			// Allocator Reset�� ȣ������� �ش� Allocator�� CommandList�� '����ϰ� ���� �ʾƾ�'�Ѵ�.
			// ID3D12GraphicsCommandList->Reset��
			// 1. Allocator�� Reset�� �޸� ������� �ҷ��� �������.
			// 2. ���°� Open�ȴ�. ���ÿ� ���ڷ� ���� CommandAllocator�� ����� �����Ѵ�.
			// �ش� Allocator�� ��ϵ� ��ɵ��� �Ȼ������. ��, Allocator�� ������ ���ݵǾ�� �츮�� ���ϴ� ������ �� ���̴�.
			// Allocator�� ������ ���� ������� ����� ��������� �ʱ�ȭ�� �ȵǾ��� ������(����) �޸𸮰� ������ �þ�� ������ �����Ǿ���.
			// �ι�° ������ PipelineState�� NULL�� ������ ���ؼ��� MSDN ����.

			// m_vShaders���� �� ���̴��� ����� �����ִ� ������ ���ҽ��� CommandAllocator�� �迭 �������� ���½�Ű�� ������
			// m_vShader�� 0��° �ε����� �ִ� Shader, �� ù��° ���̴��� CommandAllocator�� ���������� ����, CommandList�� �����ؼ� Open���� ���̴��� ���������� �������ϱ� �����̴�. 
			// ���������� �������� ���̴��� Allocator�� Framework Ŭ���� �ܰ��� Draw���� Commandlist�� Close�ϸ鼭 �ݾ��� ���̴�.
			
			// index�� 0 �̻��ε��� close�� �ϰ� �� ������ ���� ���µ� ���̴��� ������ �����ϱ� �����̴�.
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

void CScene::Update(const CTimer& timer, ID3D12Fence* pFence)
{
	
	UpdateCamera(timer.DeltaTime());


	for (auto& object : m_vObjects)
	{
		object->Update(timer.DeltaTime());
	}
	for (auto& shader : m_vShaders)
	{
		shader->Update(timer,  pFence, m_pCurrentCamera);
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

int CScene::Free(void)
{
	ReleaseScene();

	for (auto& pObject : m_vObjects)
	{
		int refCnt = Safe_Release(pObject);
	}

	Safe_Release(m_pComponent_Manager);
	return 0;
}

void CScene::ReleaseShaders(void)
{
	for (auto shader : m_vShaders)
	{
		Safe_Release(shader);
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
	if (m_pComponent_Manager == nullptr)
		return;

	CComponent* pComponent = nullptr;

	pComponent = new CTransform;
	m_pComponent_Manager->Add_Component(L"Component_Transform", pComponent);

	pComponent = new LoadModel("resources/run_Anim.FBX",m_d3dDevice.Get(), m_GraphicsCommandList.Get());
	m_pComponent_Manager->Add_Component(L"Component_Model_idle", pComponent);

	pComponent = new LoadModel("resources/xyz.FBX", m_d3dDevice.Get(), m_GraphicsCommandList.Get());
	m_pComponent_Manager->Add_Component(L"Component_Model_xyz", pComponent);

	pComponent = new LoadModel("resources/x.FBX", m_d3dDevice.Get(), m_GraphicsCommandList.Get());
	m_pComponent_Manager->Add_Component(L"Component_Model_x", pComponent);

	pComponent = new LoadModel("resources/y.FBX", m_d3dDevice.Get(), m_GraphicsCommandList.Get());
	m_pComponent_Manager->Add_Component(L"Component_Model_y", pComponent);

	pComponent = new LoadModel("resources/z.FBX", m_d3dDevice.Get(), m_GraphicsCommandList.Get());
	m_pComponent_Manager->Add_Component(L"Component_Model_z", pComponent);

	
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
