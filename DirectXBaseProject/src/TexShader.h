#ifndef _H_TEXSHADER
#define _H_TEXSHADER

///SHADER THAT HANDLES TEXTURING AND MULTITEXTURING

#include "Shader.h"
#include "Light.h"

enum TEXTURETYPE{REGULAR = 0,MULTI = 1};

class TexShader : public Shader
{
public:
	TexShader(void);

	bool Initialize(ID3D10Device* device, HWND hwnd, TEXTURETYPE texType);

	void RenderTexturing(ID3D10Device* device, int indexCount, 
													  D3DXMATRIX worldMatrix, 
													  D3DXMATRIX viewMatrix, 
													  D3DXMATRIX projectionMatrix,
													  D3DXVECTOR3 mEyePos, 
													  Light lightVar,
													  ID3D10ShaderResourceView *diffuseMap,
													  ID3D10ShaderResourceView *specularMap);

	void RenderMultiTexturing(ID3D10Device* device, int indexCount, 
													  D3DXMATRIX worldMatrix, 
													  D3DXMATRIX viewMatrix, 
													  D3DXMATRIX projectionMatrix,
													  D3DXVECTOR3 mEyePos, 
													  Light lightVar,
													  ID3D10ShaderResourceView *specularMap,
													  ID3D10ShaderResourceView *blendMap,
													  ID3D10ShaderResourceView* diffuseMapRV,
													  ID3D10ShaderResourceView* diffuseMapRV2,
													  ID3D10ShaderResourceView* diffuseMapRV3);
	~TexShader(void);

private:
	ID3D10EffectVariable* mEyePosVar;
	ID3D10EffectVariable* mLightVar;

	ID3D10EffectShaderResourceVariable* mDiffuseMap;			//for regular texturing
	ID3D10EffectShaderResourceVariable* mSpecularMap;			//for regular and mutli texturing

	ID3D10EffectShaderResourceVariable* mBlendMap;				//for multi texturing
	ID3D10EffectShaderResourceVariable* mDiffuseMapRV;			//for multi texturing
	ID3D10EffectShaderResourceVariable* mDiffuseMapRV2;			//for multi texturing
	ID3D10EffectShaderResourceVariable* mDiffuseMapRV3;			//for multi texturing

	void SetShaderParametersTexturing(int indexCount, 
							D3DXMATRIX worldMatrix, 
							D3DXMATRIX viewMatrix, 
							D3DXMATRIX projectionMatrix,
							D3DXVECTOR3 mEyePos, 
							Light lightVar,
							ID3D10ShaderResourceView *diffuseMap,
							ID3D10ShaderResourceView *specularMap);

	void SetShaderParametersMultiTexturing(int indexCount, 
											D3DXMATRIX worldMatrix, 
											D3DXMATRIX viewMatrix, 
											D3DXMATRIX projectionMatrix,
											D3DXVECTOR3 mEyePos, 
											Light lightVar,
											ID3D10ShaderResourceView *specularMap,
											ID3D10ShaderResourceView *blendMap,
											ID3D10ShaderResourceView* diffuseMapRV,
											ID3D10ShaderResourceView* diffuseMapRV2,
											ID3D10ShaderResourceView* diffuseMapRV3);

	bool InitializeShader(ID3D10Device* device, HWND hwnd, WCHAR* filename);
};

#endif