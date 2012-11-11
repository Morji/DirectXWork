#ifndef _SHADER_H_
#define _SHADER_H_

#include "d3dUtil.h"
#include <fstream>

class Shader
{
public:
	Shader(void);

	/*The functions here handle initializing and shutdown of the shader. 
	The render function sets the shader parameters and then draws the 
	prepared model vertices using the shader.*/
	bool Initialize(ID3D10Device* device, HWND hwnd);
	void Shutdown();
	void Render(ID3D10Device* device, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix);

	virtual ~Shader(void);

protected:
	bool InitializeShader(ID3D10Device* device, HWND hwnd, WCHAR* filename);

	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename);

	void SetShaderParameters(D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix);
	void RenderShader(ID3D10Device* device, int indexCount);

protected:
	ID3D10Effect* mEffect;
	ID3D10EffectTechnique* mTechnique;
	ID3D10InputLayout* mLayout;

	ID3D10EffectMatrixVariable* mWorldMatrix;
	ID3D10EffectMatrixVariable* mViewMatrix;
	ID3D10EffectMatrixVariable* mProjectionMatrix;

	ID3D10EffectMatrixVariable* mWVPMatrix;
};


#endif