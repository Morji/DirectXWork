#ifndef EXPLODESHADER_H
#define EXPLODESHADER_H

#include "LightShader.h"

class ExplodeShader : public LightShader
{
public:
	ExplodeShader(void);

	~ExplodeShader(void);

	bool Initialize(ID3D10Device* device, HWND hwnd);

	void Render(ID3D10Device* device, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, 
				D3DXVECTOR3 mEyePos, Light lightVar, int lightType, float timeStep);
private:
	bool InitializeShader(ID3D10Device* device, HWND hwnd, WCHAR* filename);

	ID3D10EffectScalarVariable* mTimeStep;	//timestep for the explosion effect
};

#endif