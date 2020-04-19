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
	BuildShaders();

	return true;
}

void CScene::BuildShaders()
{
	CShader* pShader = nullptr;

	pShader = new CShader();
	pShader->Initialize(m_d3dDevice.Get(),m_GraphicsCommandList.Get());
	m_vShaders.push_back(pShader);
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
}

void CScene::Update(const CTimer& timer, ID3D12Fence* pFence, ID3D12GraphicsCommandList * cmdList)
{
	OnKeyboardInput(timer.DeltaTime());
	UpdateCamera(timer.DeltaTime());

	for (auto& shader : m_vShaders)
	{
		shader->Update(timer,  pFence, cmdList);
	}
	
}

void CScene::Render(UINT frameResourceIndex, ID3D12GraphicsCommandList * cmdList, UINT CbvSrvUavDescriptorSize)
{

	for (auto shader : m_vShaders)
	{
		shader->Render(frameResourceIndex, cmdList, nullptr);
	}
}

void CScene::ReleaseUploadBuffers()
{
}

void CScene::OnResize(float fAspectRatio)
{
	if (m_pCurrentCamera)
		m_pCurrentCamera->GenerateProjectionMatrix(1.0f, 1000.f, fAspectRatio, 0.25 * MathHelper::Pi);

}


void CScene::BuildDescriptorHeaps()
{
}

void CScene::BuildConstantBufferViews(UINT CbvSrvUavDescriptorSize)
{
	
}

void CScene::BuildComponents(void)
{
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

void CScene::BuildObject(void)
{
	
}
