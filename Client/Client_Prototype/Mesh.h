#pragma once
#include "stdafx.h"

class CMesh
{
public:
	CMesh(ID3D12Device* pDevice,ID3D12GraphicsCommandList* pCommandList);
	virtual ~CMesh();

public:
	virtual void BuildMeshGeo(void);

	MeshGeometry* GetMeshGeo(void) {
		return m_MeshGeo.get();
	};
protected:
	ComPtr<ID3D12Device> m_d3dDevice;
	ComPtr<ID3D12GraphicsCommandList> m_GraphicsCommandList;

	unique_ptr<MeshGeometry> m_MeshGeo;
};

class CBoxMesh : public CMesh 
{
public:
	CBoxMesh(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList);
	virtual ~CBoxMesh();

	virtual void BuildMeshGeo(void);
};