

//=======================================================================================
// Light.h by Frank Luna (C) 2008 All Rights Reserved.
//=======================================================================================

#ifndef LIGHT_H
#define LIGHT_H

#include "d3dUtil.h"

enum LIGHT_TYPE{L_PARALLEL = 0,L_POINT = 1,L_SPOT = 2};

struct Light{

	Light(){
		ZeroMemory(this, sizeof(Light));
	}

	D3DXVECTOR3 pos;
	float pad4;      // not used
	D3DXVECTOR3 dir;
	float pad5;      // not used
	D3DXCOLOR ambient;
	D3DXCOLOR diffuse;
	D3DXCOLOR specular;
	D3DXVECTOR3 att;
	float spotPow;
	float range;
};

#endif // LIGHT_H