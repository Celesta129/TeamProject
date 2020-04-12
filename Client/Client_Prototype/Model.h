#pragma once
#include "stdafx.h"
#include "Mesh.h"

class CModel
{
public:
	CModel();
	~CModel();

private:
	vector<CMesh> meshes;
};

