#include "Scene.h"

#include "Timer.h"
#include "Camera.h"

#include "ObjectShader.h"
#include "AxisShader.h"
#include "UI_Shader.h"
#include "Object_TextureShader.h"
#include "CShader_Test.h"
#include "ModelObject.h"

CScene::CScene(ComPtr<ID3D12Device> pDevice, ComPtr<ID3D12GraphicsCommandList> pCommandList)
	:m_d3dDevice(pDevice), m_GraphicsCommandList(pCommandList)
{
	m_pComponent_Manager = CComponent_Manager::GetInstance();
	m_pComponent_Manager->AddRef();

	m_pObject_Manager = CObject_Manager::GetInstance();
	m_pObject_Manager->AddRef();
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

void CScene::Set_Camera(UINT index)
{
	vector<CGameObject*>* pPlayerLayer = m_pObject_Manager->Get_Layer(CObject_Manager::LAYER_PLAYER);
	if (index > pPlayerLayer->size())
	{
		return;
	}
	if ((*pPlayerLayer)[index] != nullptr)
	{
		m_pCurrentCamera->SetTarget(m_pObject_Manager->Get_Object(CObject_Manager::LAYER_PLAYER,index));
	}
}

void CScene::BuildShaders()
{
	CShader* pShader = nullptr;
	CModelObject* pObject = nullptr;
	CTransform* pTransform = nullptr;

	
	vector<CGameObject*>* pPlayerLayer = m_pObject_Manager->Get_Layer(CObject_Manager::LAYER_PLAYER);
	vector<CGameObject*>* pObjectLayer = m_pObject_Manager->Get_Layer(CObject_Manager::LAYER_OBJECT);
	vector<CGameObject*>* pUILayer = m_pObject_Manager->Get_Layer(CObject_Manager::LAYER_UI);
	// For TextureObject
	pShader = new CObject_TextureShader;
	pShader->Initialize(m_d3dDevice.Get(), m_GraphicsCommandList.Get(), L"Shaders\\TexObject.hlsl", *pPlayerLayer);
	m_vShaders.push_back(pShader);
	for (int i = 0; i < MAX_USER; ++i)
	{
		m_player[i] = (CPlayer*)(*pPlayerLayer)[i];
	}

	pShader = new Shader_Test;
	pShader->Initialize(m_d3dDevice.Get(), m_GraphicsCommandList.Get(), L"Shaders\\TextureModel.hlsl", *pObjectLayer);
	m_vShaders.push_back(pShader);

	pShader = new CUI_Shader;
	pShader->Initialize(m_d3dDevice.Get(), m_GraphicsCommandList.Get(), L"Shaders\\UIShader.hlsl", *pUILayer);
	m_vShaders.push_back(pShader);
	//pObject = new CModelObject;
	//dynamic_cast<CModelObject*>(pObject)->Initialize(L"Component_Model_sandbox", m_d3dDevice.Get(), m_GraphicsCommandList.Get());
	//m_vObjects.push_back(pObject);		// ��ü ������Ʈ ���� ���Ϳ� �ִ´�.
	//pShader->Push_Object(pObject);		// ���� ���̴����� �ִ´�.
	//dynamic_cast<CObject_TextureShader*>(pShader)->setMat(pObject, 1);
}

bool CScene::OnKeyboardInput(const float & fTimeElapsed)
{
	if (GetAsyncKeyState('1') & 0x8000)
	{
		vector<CGameObject*>* pvWeapon = m_pObject_Manager->Get_Layer(CObject_Manager::LAYER_WEAPON);
		CWeapon* pWeapon = new CWeapon();
		
		pWeapon->Initialize(m_d3dDevice.Get(), m_GraphicsCommandList.Get());
		pWeapon->set_Type(rand() % 4);
		pvWeapon->push_back(pWeapon);
		pWeapon->Get_Transform()->Set_Pos(XMFLOAT3(300.f,0.f,300.f));
		m_vShaders[1]->Push_Object(pWeapon);
	}
	if (GetAsyncKeyState('2') & 0x8000)
	{
		
	}
	if (GetAsyncKeyState('3') & 0x8000)
	{
		
	}

	/*float fSpeed = 500.f * fTimeElapsed;
	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		if (m_vObjects[0] == nullptr)
			return false;

		m_vObjects[0]->Get_Transform()->MovePos(XMFLOAT3(-fSpeed,0.f,0.f));
		m_vObjects[0]->DirtyFrames();
	}
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		if (m_vObjects[0] == nullptr)
			return false;

		m_vObjects[0]->Get_Transform()->MovePos(XMFLOAT3(fSpeed, 0.f, 0.f));
		m_vObjects[0]->DirtyFrames();
	}
	if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		if (m_vObjects[0] == nullptr)
			return false;

		m_vObjects[0]->Get_Transform()->MovePos(XMFLOAT3(0.f, 0.f, fSpeed));
		m_vObjects[0]->DirtyFrames();
	}
	if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		if (m_vObjects[0] == nullptr)
			return false;

		m_vObjects[0]->Get_Transform()->MovePos(XMFLOAT3(0.f, 0.f, -fSpeed));
		m_vObjects[0]->DirtyFrames();
	}*/
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
	float fMax_Radius = 500.f;
	float fMin_Radius = 500.f;


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

int CScene::Update(const CTimer& timer, ID3D12Fence* pFence)
{
	int result = 0;
	UpdateCamera(timer.DeltaTime());

	for (UINT i = 0; i < CObject_Manager::LAYER_END; ++i)
	{
		for (auto& object : *m_pObject_Manager->Get_Layer(i))
		{
			result = object->Update(timer.DeltaTime());
			if (result == UPDATE_TIMEOUT)
				cout << "timeout" << endl;
		}
	}
	
	for (auto& shader : m_vShaders)
	{
		result = shader->Update(timer,  pFence, m_pCurrentCamera);
	}
	
	return 0;
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

CShader * CScene::Get_Shader(UINT index)
{
	CShader* pShader = nullptr;
	if(index > m_vShaders.size())
		return pShader;

	return &pShader[index];
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
	Safe_Release(m_pObject_Manager);
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

	//pComponent = new LoadModel("resources/normal_attack_1.FBX", m_d3dDevice.Get(), m_GraphicsCommandList.Get());
	//m_pComponent_Manager->Add_Component(L"Component_Model_normalattack1", pComponent);

	//pComponent = new LoadModel("resources/normal_attack_2.FBX", m_d3dDevice.Get(), m_GraphicsCommandList.Get());
	//m_pComponent_Manager->Add_Component(L"Component_Model_normalattack2", pComponent);

	//Ÿ��
	pComponent = new LoadModel("resources/hit_Anim.FBX", m_d3dDevice.Get(), m_GraphicsCommandList.Get());
	m_pComponent_Manager->Add_Component(L"Component_Model_hit", pComponent);
	
	pComponent = new CMaterial(L"resources/character_test.dds", L"Texture_character", m_d3dDevice.Get(), m_GraphicsCommandList.Get());
	m_pComponent_Manager->Add_Component(L"Texture_character", pComponent);
	
	pComponent = new LoadModel("resources/fbx/bench.FBX", m_d3dDevice.Get(), m_GraphicsCommandList.Get());
	m_pComponent_Manager->Add_Component(L"Component_Model_bench",pComponent);
	pComponent = new CMaterial(L"resources/dds/Bench_diffuse.dds", L"Texture_bench", m_d3dDevice.Get(), m_GraphicsCommandList.Get());
	m_pComponent_Manager->Add_Component(L"Texture_bench", pComponent);

	pComponent = new LoadModel("resources/fbx/brick_wall_A.FBX", m_d3dDevice.Get(), m_GraphicsCommandList.Get());
	m_pComponent_Manager->Add_Component(L"Component_Model_brick_wall_A", pComponent);
	pComponent = new CMaterial(L"resources/dds/Brickwall_A_diffuse.dds", L"Texture_brick_wall_A", m_d3dDevice.Get(), m_GraphicsCommandList.Get());
	m_pComponent_Manager->Add_Component(L"Texture_brick_wall_A", pComponent);

	pComponent = new LoadModel("resources/fbx/brick_wall_B.FBX", m_d3dDevice.Get(), m_GraphicsCommandList.Get());
	m_pComponent_Manager->Add_Component(L"Component_Model_brick_wall_B", pComponent);
	pComponent = new CMaterial(L"resources/dds/Brickwall_B_diffuse.dds", L"Texture_brick_wall_B", m_d3dDevice.Get(), m_GraphicsCommandList.Get());
	m_pComponent_Manager->Add_Component(L"Texture_brick_wall_B", pComponent);

	pComponent = new LoadModel("resources/map_playground_floor.FBX", m_d3dDevice.Get(), m_GraphicsCommandList.Get());
	m_pComponent_Manager->Add_Component(L"Component_Model_Floor", pComponent);
	pComponent = new CMaterial(L"resources/map_playground_floor_test.dds", L"Texture_Floor", m_d3dDevice.Get(), m_GraphicsCommandList.Get());
	m_pComponent_Manager->Add_Component(L"Texture_Floor", pComponent);

	pComponent = new LoadModel("resources/fbx/SandBox.FBX", m_d3dDevice.Get(), m_GraphicsCommandList.Get());
	m_pComponent_Manager->Add_Component(L"Component_Model_SandBox", pComponent);
	pComponent = new CMaterial(L"resources/dds/SandBox_diffuse.dds", L"Texture_SandBox", m_d3dDevice.Get(), m_GraphicsCommandList.Get());
	m_pComponent_Manager->Add_Component(L"Texture_SandBox", pComponent);

	pComponent = new LoadModel("resources/fbx/seesaw.FBX", m_d3dDevice.Get(), m_GraphicsCommandList.Get());
	m_pComponent_Manager->Add_Component(L"Component_Model_SeeSaw", pComponent);
	pComponent = new CMaterial(L"resources/dds/seesaw_diffuse.dds", L"Texture_SeeSaw", m_d3dDevice.Get(), m_GraphicsCommandList.Get());
	m_pComponent_Manager->Add_Component(L"Texture_SeeSaw", pComponent);

	pComponent = new LoadModel("resources/fbx/Hobby_horse.FBX", m_d3dDevice.Get(), m_GraphicsCommandList.Get());
	m_pComponent_Manager->Add_Component(L"Component_Model_HobbyHorse", pComponent);
	pComponent = new CMaterial(L"resources/dds/Hobby_horse_Diffuse_Red.dds", L"Texture_HobbyHorse_red", m_d3dDevice.Get(), m_GraphicsCommandList.Get());
	m_pComponent_Manager->Add_Component(L"Texture_HobbyHorse_red", pComponent);
	pComponent = new CMaterial(L"resources/dds/Hobby_horse_Diffuse_yellow.dds", L"Texture_HobbyHorse_yellow", m_d3dDevice.Get(), m_GraphicsCommandList.Get());
	m_pComponent_Manager->Add_Component(L"Texture_HobbyHorse_yellow", pComponent);

	pComponent = new LoadModel("resources/fbx/map_billboard.FBX", m_d3dDevice.Get(), m_GraphicsCommandList.Get());
	m_pComponent_Manager->Add_Component(L"Component_Model_BillBoard", pComponent);
	pComponent = new CMaterial(L"resources/dds/map_billboard.dds", L"Texture_BillBoard", m_d3dDevice.Get(), m_GraphicsCommandList.Get());
	m_pComponent_Manager->Add_Component(L"Texture_BillBoard", pComponent);

	pComponent = new LoadModel("resources/fbx/flag.FBX", m_d3dDevice.Get(), m_GraphicsCommandList.Get());
	m_pComponent_Manager->Add_Component(L"Component_Model_Flag", pComponent);
	pComponent = new CMaterial(L"resources/dds/flag_diffuse.dds", L"Texture_Flag", m_d3dDevice.Get(), m_GraphicsCommandList.Get());
	m_pComponent_Manager->Add_Component(L"Texture_Flag", pComponent);

	pComponent = new LoadModel("resources/fbx/1x1_plane.FBX", m_d3dDevice.Get(), m_GraphicsCommandList.Get());
	m_pComponent_Manager->Add_Component(L"Component_Model_UIPlane", pComponent);

	pComponent = new CMaterial(L"resources/dds/HPedge.dds", L"Texture_HPedge", m_d3dDevice.Get(), m_GraphicsCommandList.Get());
	m_pComponent_Manager->Add_Component(L"Texture_HPedge", pComponent);
	pComponent = new CMaterial(L"resources/dds/HPbar.dds", L"Texture_HPbar", m_d3dDevice.Get(), m_GraphicsCommandList.Get());
	m_pComponent_Manager->Add_Component(L"Texture_HPbar", pComponent);

	
	pComponent = new LoadModel("resources/fbx/hammer.FBX", m_d3dDevice.Get(), m_GraphicsCommandList.Get());
	m_pComponent_Manager->Add_Component(L"Component_Model_Hammer", pComponent);
	pComponent = new CMaterial(L"resources/dds/hammer_diffuse.dds", L"Texture_Hammer_diffuse", m_d3dDevice.Get(), m_GraphicsCommandList.Get());
	m_pComponent_Manager->Add_Component(L"Texture_Hammer_diffuse", pComponent);

	pComponent = new LoadModel("resources/fbx/sword.FBX", m_d3dDevice.Get(), m_GraphicsCommandList.Get());
	m_pComponent_Manager->Add_Component(L"Component_Model_Sword", pComponent);
	pComponent = new CMaterial(L"resources/dds/sword_diffuse.dds", L"Texture_Sword_diffuse", m_d3dDevice.Get(), m_GraphicsCommandList.Get());
	m_pComponent_Manager->Add_Component(L"Texture_Sword_diffuse", pComponent);

	pComponent = new LoadModel("resources/fbx/snack.FBX", m_d3dDevice.Get(), m_GraphicsCommandList.Get());
	m_pComponent_Manager->Add_Component(L"Component_Model_Snack", pComponent);
	pComponent = new CMaterial(L"resources/dds/snack_diffuse.dds", L"Texture_Snack_diffuse", m_d3dDevice.Get(), m_GraphicsCommandList.Get());
	m_pComponent_Manager->Add_Component(L"Texture_Snack_diffuse", pComponent);

	pComponent = new LoadModel("resources/fbx/block.FBX", m_d3dDevice.Get(), m_GraphicsCommandList.Get());
	m_pComponent_Manager->Add_Component(L"Component_Model_Block", pComponent);
	pComponent = new CMaterial(L"resources/dds/block_red_diffuse.dds", L"Texture_Block_diffuse", m_d3dDevice.Get(), m_GraphicsCommandList.Get());
	m_pComponent_Manager->Add_Component(L"Texture_Block_diffuse", pComponent);
}


void CScene::BuildCamera(void)
{
	for (int i = 0; i < MAX_USER; ++i) {
		m_vCameras.push_back(new CCamera);
		m_vCameras[i]->SetTarget(m_pObject_Manager->Get_Object(CObject_Manager::LAYER_PLAYER,i));
	}
	m_pCurrentCamera = m_vCameras[0];
}

void CScene::BuildObject(void)
{

}
