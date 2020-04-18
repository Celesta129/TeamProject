#include "stdafx.h"
#include "Scene.h"
#include "D3DApp.h"
#include "Camera.h"
#include "Mesh.h"
#include "../Common/GeometryGenerator.h"
#include "Shader.h"

CScene::CScene(ComPtr<ID3D12Device> pDevice, ComPtr<ID3D12GraphicsCommandList> pCommandList)
	:m_d3dDevice(pDevice), m_GraphicsCommandList(pCommandList)
{
}


CScene::~CScene()
{
	if (m_pCamera)
		delete m_pCamera;
	if (m_pBoxMesh)
		delete m_pBoxMesh;
}

bool CScene::Initialize(UINT CbvSrvUavDescriptorSize)
{
	m_pComponent_Manager = CComponent_Manager::GetInstance();
	m_vObjects.reserve(MAX_OBJECT);

	return true;
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
	
}

void CScene::Update(float fTimeElapsed, ID3D12Fence* pFence)
{
	m_CurrentFrameResourceIndex = (m_CurrentFrameResourceIndex + 1) % NumFrameResources;
	m_CurrentFrameResource = m_vFrameResources[m_CurrentFrameResourceIndex].get();

	//gpu�� ���� ������ �ڿ��� ����� �� ó���ߴ��� Ȯ��.
	// ���� �� ó�������ʾ����� gpu�� �� �潺�������� ��ɵ��� ó���Ҷ����� ��ٸ���.
	// �ᱹ ���� ��İ� �ٺ������� �ٸ��� ������ ��� ó���� �ڿ��� 3���� ����� �� GPU�۾��� �Ϸ�ɋ����� CPU�� �����ʰ� �ٸ� ��� ������ �Ѵٴ� ���������ִ�.
	if (m_CurrentFrameResource->Fence != 0 && pFence->GetCompletedValue() < m_CurrentFrameResource->Fence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);
		ThrowIfFailed(pFence->SetEventOnCompletion(m_CurrentFrameResource->Fence, eventHandle));
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
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
	if (m_pCamera)
		m_pCamera->GenerateProjectionMatrix(1.0f, 1000.f, fAspectRatio, 0.25 * MathHelper::Pi);

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
	for (int i = 0; i < gNumFrameResources; ++i)
	{
		m_vFrameResources.push_back(make_unique<FrameResource>(m_d3dDevice.Get(), 1, MAX_OBJECT));
	}
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
