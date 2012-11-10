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
	void init (ID3D10Device* device, DWORD nVertices, DWORD nFaces);
	//virtual void update(float dt);
	void draw(ID3D10EffectTechnique* mTech); 

protected:
	bool SetupArraysAndInitBuffers(ID3D10Device* device);
};

