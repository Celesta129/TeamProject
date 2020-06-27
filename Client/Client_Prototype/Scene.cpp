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
		m_player[i] = new CPlayer();	// 이부분 수정요망. 뭔지는 내가 보면 암.
		m_player[i]->SetObjectInstance((CModelObject*)(*pPlayerLayer)[i]);
	}

	pShader = new Shader_Test;
	pShader->Initialize(m_d3dDevice.Get(), m_GraphicsCommandList.Get(), L"Shaders\\TextureModel.hlsl", *pObjectLayer);
	m_vShaders.push_back(pShader);

	pShader = new CUI_Shader;
	pShader->Initialize(m_d3dDevice.Get(), m_GraphicsCommandList.Get(), L"Shaders\\UIShader.hlsl", *pUILayer);
	m_vShaders.push_back(pShader);
	//pObject = new CModelObject;
	//dynamic_cast<CModelObject*>(pObject)->Initialize(L"Component_Model_sandbox", m_d3dDevice.Get(), m_GraphicsCommandList.Get());
	//m_vObjects.push_back(pObject);		// 전체 오브젝트 관리 벡터에 넣는다.
	//pShader->Push_Object(pObject);		// 개별 셰이더에도 넣는다.
	//dynamic_cast<CObject_TextureShader*>(pShader)->setMat(pObject, 1);
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
			// Allocator Reset은 호출시점의 해당 Allocator를 CommandList가 '사용하고 있지 않아야'한다.
			// ID3D12GraphicsCommandList->Reset은
			// 1. Allocator의 Reset과 달리 어느때나 불러도 상관없다.
			// 2. 상태가 Open된다. 동시에 인자로 받은 CommandAllocator에 기록을 시작한다.
			// 해당 Allocator에 기록된 명령들은 안사라진다. 즉, Allocator의 리셋이 동반되어야 우리가 원하는 리셋이 될 것이다.
			// Allocator의 리셋을 하지 않을경우 기록은 계속했지만 초기화가 안되었기 때문에(추측) 메모리가 무한정 늘어나는 현상이 관찰되었다.
			// 두번째 인자인 PipelineState가 NULL인 이유에 대해서는 MSDN 참고.

			// m_vShaders안의 각 셰이더가 멤버로 갖고있는 프레임 리소스의 CommandAllocator를 배열 역순으로 리셋시키는 이유는
			// m_vShader의 0번째 인덱스에 있는 Shader, 즉 첫번째 셰이더의 CommandAllocator를 마지막으로 리셋, CommandList에 세팅해서 Open시켜 셰이더를 순차적으로 렌더링하기 위함이다. 
			// 마지막으로 렌더링한 셰이더의 Allocator는 Framework 클래스 단계의 Draw에서 Commandlist를 Close하면서 닫았을 것이다.
			
			// index가 0 이상인동안 close를 하게 한 이유는 다음 리셋될 셰이더의 리셋을 보장하기 위함이다.
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

	//타격
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
