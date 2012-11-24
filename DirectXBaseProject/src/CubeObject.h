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

private:
	bool SetupArraysAndInitBuffers();
};

