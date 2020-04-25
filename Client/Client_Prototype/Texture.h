#pragma once
#include "stdafx.h"
#include "Base.h"

#define RESOURCE_TEXTURE2D				0x01
#define RESOURCE_TEXTURE2D_ARRAY		0x02	
#define RESOURCE_TEXTURE2DARRAY			0x03
#define RESOURCE_TEXTURE_CUBE			0x04
#define RESOURCE_BUFFER					0x05
#define RESOURCE_TEXTURE2D_SHADOWMAP	0x06
#define RESOURCE_TEXTURE2D_HDR			0x07
#define RESOURCE_BUFFER_FLOAT32			0x08

class CTexture : public CBase
{
public:
	CTexture();
	~CTexture();
public:
	HRESULT Initialize(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pCommandList, string Name, wstring Filename);

	virtual int Free();

private:
	Texture* m_pTexture = nullptr;
};

