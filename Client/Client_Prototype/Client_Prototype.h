#pragma once

#include "resource.h"

#include "D3DApp.h"

class CScene;

class CGameFramework_Client : public CD3DApp
{
public:
	CGameFramework_Client();
	~CGameFramework_Client();

	virtual bool Initialize(HINSTANCE hInstance) override;

private:
	virtual void OnResize() override;
	virtual void Update(CTimer* const gt)override;
	virtual void Draw(CTimer* const gt)override;

private:

	LPTSTR WindowCaption;

private:
	CScene* m_pScene = nullptr;
};

