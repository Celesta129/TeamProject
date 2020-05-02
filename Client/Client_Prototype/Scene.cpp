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

	
	// For TextureObject
	pShader = new CObject_TextureShader;
	pShader->Initialize(m_d3dDevice.Get(), m_GraphicsCommandList.Get(), L"Shaders\\TexObject.hlsl");
	m_vShaders.push_back(pShader);


	for (int i = 0; i < MAX_USER; ++i)
	{
		pObject = new CModel_TextureObject;
		if(i % 3 == 0)
			dynamic_cast<CModel_TextureObject*>(pObject)->Initialize(L"Component_Model_idle", m_d3dDevice.Get(), m_GraphicsCommandList.Get());
		else if(i % 3 == 1)
			dynamic_cast<CModel_TextureObject*>(pObject)->Initialize(L"Component_Model_run", m_d3dDevice.Get(), m_GraphicsCommandList.Get());
		else
			dynamic_cast<CModel_TextureObject*>(pObject)->Initialize(L"Component_Model_attack", m_d3dDevice.Get(), m_GraphicsCommandList.Get());
		m_vObjects.push_back(pObject);		// ��ü ������Ʈ ���� ���Ϳ� �ִ´�.
		pShader->Push_Object(pObject);		// ���� ���̴����� �ִ´�.
		
		dynamic_cast<CObject_TextureShader*>(pShader)->setMat(pObject, 0);

		pTransform = GET_COMPONENT(CTransform*, pObject, L"Component_Transform");
		pTransform->MovePos(XMFLOAT3(50.f * i, 0.f, 0.f));

		m_player[i] = new CPlayer();
		m_player[i]->SetObjectInstance(pObject);
	}

	pObject = new CModel_TextureObject;
	dynamic_cast<CModel_TextureObject*>(pObject)->Initialize(L"Component_Model_sandbox", m_d3dDevice.Get(), m_GraphicsCommandList.Get());
	m_vObjects.push_back(pObject);		// ��ü ������Ʈ ���� ���Ϳ� �ִ´�.
	pShader->Push_Object(pObject);		// ���� ���̴����� �ִ´�.
	dynamic_cast<CObject_TextureShader*>(pShader)->setMat(pObject, 1);
}

bool CScene::OnKeyboardInput(const float & fTimeElapsed)
{
	if (GetAsyncKeyState('1') & 0x8000)
	{
		if (m_vCameras[0])
			m_pCurrentCamera = m_vCameras[0];
	}
	if (GetAsyncKeyState('2') & 0x8000)
	{
		if (m_vCameras[1])
			m_pCurrentCamera = m_vCameras[1];
	}
	if (GetAsyncKeyState('3') & 0x8000)
	{
		if (m_vCameras[2])
			m_pCurrentCamera = m_vCameras[2];
	}
	if (GetAsyncKeyState('4') & 0x8000)
	{
		if (m_vCameras[3])
			m_pCurrentCamera = m_vCameras[3];
	}
	if (GetAsyncKeyState('5') & 0x8000)
	{
		if (m_vCameras[4])
			m_pCurrentCamera = m_vCameras[4];
	}
	if (GetAsyncKeyState('6') & 0x8000)
	{
		if (m_vCameras[5])
			m_pCurrentCamera = m_vCameras[5];
	}
	if (GetAsyncKeyState('7') & 0x8000)
	{
		if (m_vCameras[6])
			m_pCurrentCamera = m_vCameras[6];
	}
	if (GetAsyncKeyState('8') & 0x8000)
	{
		if (m_vCameras[7])
			m_pCurrentCamera = m_vCameras[7];
	}
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

void CScene::OnResize(float fAspectRatio)
{
	m_pCurrentCamera->Update(0);
}

void CScene::ReleaseScene(void)
{
	ReleaseShaders();
	ReleaseCameras();
	ReleaseObjects();
}

int CScene::Free(void)
{
	ReleaseScene();

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

}

void CScene::BuildComponents(void)
{
	if (m_pComponent_Manager == nullptr)
		return;

	CComponent* pComponent = nullptr;

	pComponent = new CTransform;
	m_pComponent_Manager->Add_Component(L"Component_Transform", pComponent);

	pComponent = new LoadModel("resources/idle_Anim.FBX",m_d3dDevice.Get(), m_GraphicsCommandList.Get());
	m_pComponent_Manager->Add_Component(L"Component_Model_idle", pComponent);

	pComponent = new LoadModel("resources/run_Anim.FBX", m_d3dDevice.Get(), m_GraphicsCommandList.Get());
	m_pComponent_Manager->Add_Component(L"Component_Model_run", pComponent);

	pComponent = new LoadModel("resources/attack_Anim.FBX", m_d3dDevice.Get(), m_GraphicsCommandList.Get());
	m_pComponent_Manager->Add_Component(L"Component_Model_attack", pComponent);
	
	pComponent = new LoadModel("resources/fbx/xyz.FBX", m_d3dDevice.Get(), m_GraphicsCommandList.Get());
	m_pComponent_Manager->Add_Component(L"Component_Model_sandbox", pComponent);
}


void CScene::BuildCamera(void)
{
	for (int i = 0; i < MAX_USER; ++i) {
		m_vCameras.push_back(new CCamera);
		m_vCameras[i]->SetTarget(m_vObjects[i]);
	}
	m_pCurrentCamera = m_vCameras[0];
}

void CScene::BuildObject(void)
{

}
