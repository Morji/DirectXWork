#ifndef _LIGHTSHADER_H_
#define _LIGHTSHADER_H_

#include "Shader.h"
#include "Light.h"

class LightShader :	public Shader {

public:	
	LightShader(void);
	~LightShader(void);

	bool Initialize(ID3D10Device* device, HWND hwnd);

	void Render(ID3D10Device* device, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, 
				D3DXVECTOR3 mEyePos, Light lightVar, int lightType);

	

private:
	ID3D10EffectVariable* mEyePosVar;
	ID3D10EffectVariable* mLightVar;
	ID3D10EffectScalarVariable* mLightType;	

	void SetShaderParameters(D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix,
							 D3DXVECTOR3 mEyePos, Light lightVar, int lightType);

	bool InitializeShader(ID3D10Device* device, HWND hwnd, WCHAR* filename);
};

#endif