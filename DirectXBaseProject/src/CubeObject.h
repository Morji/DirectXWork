#pragma once

#include "GameObject.h"

class CubeObject : public GameObject
{
public:
	CubeObject(){
	}
	~CubeObject(){
		Shutdown();
	}

protected:
	bool SetupArraysAndInitBuffers(ID3D10Device* device);
};

