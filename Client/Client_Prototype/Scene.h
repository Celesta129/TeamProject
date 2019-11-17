#pragma once
class CGameObject;
class CCamera;

class CScene
{
public:
	CScene(ComPtr<ID3D12Device> pDevice);
	virtual ~CScene();

	bool Initialize();

	//������ ���콺�� Ű���� �޽����� ó���Ѵ�. 
	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	void ReleaseObjects();

	void AnimateObjects(float fTimeElapsed);
	void Render(ID3D12GraphicsCommandList *pd3dCommandList);

	void ReleaseUploadBuffers();

	//�׷��� ��Ʈ �ñ׳��ĸ� �����Ѵ�. 
	ComPtr<ID3D12RootSignature> CreateGraphicsRootSignature(ID3D12Device *pd3dDevice);
	ComPtr<ID3D12RootSignature> GetGraphicsRootSignature();
protected:
	void BuildDescriptorHeaps(void);
	void BuildConstantBuffers(void);
	void BuildRootSignature(void);
	void BuildShadersAndInputLayout(void);
	void BuildBoxGeometry(void);
	void BuildPSO(void);
protected:
	ComPtr<ID3D12Device> m_d3dDevice;

	//��ġ(Batch) ó���� �ϱ� ���Ͽ� ���� ���̴����� ����Ʈ�� ǥ���Ѵ�. 
	//CObjectShader *m_pShaders = NULL;
	int m_nShaders = 0;

	ComPtr<ID3D12RootSignature> m_d3dGraphicsRootSignature = nullptr;
	ComPtr<ID3D12DescriptorHeap> m_cbvHeap = nullptr;		// ConstantBufferViewHeap
	


	ComPtr<ID3DBlob> mvsByteCode = nullptr;		// Vertex Shader
	ComPtr<ID3DBlob> mpsByteCode = nullptr;		// Pixel Shader

	vector<D3D12_INPUT_ELEMENT_DESC> m_InputLayout;

	ComPtr<ID3D12PipelineState> m_PSO = nullptr;		// Pipeline State Object


protected:
	CCamera* m_pCamera = nullptr;
};

