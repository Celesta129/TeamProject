#pragma once

#include "resource.h"

#include "D3DApp.h"
class CGameFramework_Client : public CD3DApp
{
public:
	CGameFramework_Client(HINSTANCE hInstance);
	~CGameFramework_Client();

	virtual bool Initialize() override;

private:
	virtual void OnResize() override;
	virtual void Update(CTimer* const gt)override;
	virtual void Draw(CTimer* const gt)override;

private:

	LPTSTR WindowCaption;
};

