#pragma once
#include "Scene.h"

class CObjectsShader;
class CMap_1 : public CScene
{
public:
	CMap_1(ComPtr<ID3D12Device> pDevice, ComPtr<ID3D12GraphicsCommandList> pCommandList);
	virtual ~CMap_1();
public:
	virtual bool Initialize(UINT CbvSrvUavDescriptorSize);

	//씬에서 마우스와 키보드 메시지를 처리한다. 
	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseObjects();

	virtual void AnimateObjects(float fTimeElapsed);

	virtual void Update(float fTimeElapsed, ID3D12Fence* pFence);
	virtual void Render(UINT CbvSrvUavDescriptorSize);

protected:
	virtual void BuildComponents(void);
	virtual void BuildRootSignature(void);
	virtual void BuildDescriptorHeaps(void);
	virtual void BuildConstantBufferViews(UINT CbvSrvUavDescriptorSize);
	virtual void BuildShadersAndInputLayout(void);
	virtual void BuildPSOs(void);
	virtual void BuildFrameResources(void);
	virtual void BuildShapeGeometry(void);
	virtual void BuildRenderItems(void);

	virtual void BuildMesh(void);
	virtual void BuildObject(void);

private:
	CObjectsShader* m_pObjectsShader = nullptr;
};

