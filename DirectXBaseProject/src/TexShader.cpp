#include "TexShader.h"


TexShader::TexShader(void)
{
}


TexShader::~TexShader(void)	
{
}

bool TexShader::Initialize(ID3D10Device* device, HWND hwnd, TEXTURETYPE texType){

	bool result;

	// Initialize the shader that will be used to draw the triangle.
	//they initialize the same shader now, but later we will move multitexturing in its own shader
	switch (texType){
	case REGULAR:
		result = InitializeShader(device, hwnd, L"assets/texture.fx");
		break;
	case MULTI:
		result = InitializeShader(device, hwnd, L"assets/multitexture.fx");
		break;
	}

	if(!result){
		return false;
	}

	return true;
}

void TexShader::RenderTexturing(ID3D10Device* device, int indexCount, D3DXMATRIX worldMatrix, 
													  D3DXMATRIX viewMatrix, 
													  D3DXMATRIX projectionMatrix,
													  D3DXVECTOR3 mEyePos, 
													  Light lightVar,
													  ID3D10ShaderResourceView *diffuseMap,
													  ID3D10ShaderResourceView *specularMap)
{

	// Set the shader parameters that it will use for rendering.
	SetShaderParametersTexturing(indexCount, worldMatrix, viewMatrix, projectionMatrix, mEyePos, lightVar, diffuseMap, specularMap);

	// Now render the prepared buffers with the shader.
	RenderShader(device, indexCount);
}

void TexShader::RenderMultiTexturing(ID3D10Device* device, int indexCount, 
													  D3DXMATRIX worldMatrix, 
													  D3DXMATRIX viewMatrix, 
													  D3DXMATRIX projectionMatrix,
													  D3DXVECTOR3 mEyePos, 
													  Light lightVar,
													  ID3D10ShaderResourceView *specularMap,
													  ID3D10ShaderResourceView *blendMap,
													  ID3D10ShaderResourceView* diffuseMapRV1,
													  ID3D10ShaderResourceView* diffuseMapRV2,
													  ID3D10ShaderResourceView* diffuseMapRV3,
													  float maxHeight,
													  int lightType){

	// Set the shader parameters that it will use for rendering.
	SetShaderParametersMultiTexturing(indexCount, worldMatrix, viewMatrix, projectionMatrix, mEyePos, lightVar, specularMap, blendMap,diffuseMapRV1,diffuseMapRV2,diffuseMapRV3,maxHeight,lightType);

	// Now render the prepared buffers with the shader.
	RenderShader(device, indexCount);
}

void TexShader::SetShaderParametersTexturing(int indexCount, 
									D3DXMATRIX worldMatrix, 
									D3DXMATRIX viewMatrix, 
									D3DXMATRIX projectionMatrix,
									D3DXVECTOR3 mEyePos, 
									Light lightVar,
									ID3D10ShaderResourceView *diffuseMap,
									ID3D10ShaderResourceView *specularMap)
{

	Shader::SetShaderParameters(worldMatrix,viewMatrix,projectionMatrix);

	// Set the eye position variable inside the shader
	mEyePosVar->SetRawValue(&mEyePos, 0, sizeof(D3DXVECTOR3));

	// Set the light variable inside the shader
	mLightVar->SetRawValue(&lightVar, 0, sizeof(Light));

	// Set the diffuse map shader var
	mDiffuseMap->SetResource(diffuseMap);

	// Set the specular map shader var
	mSpecularMap->SetResource(specularMap);
}

void TexShader::SetShaderParametersMultiTexturing(int indexCount, 
											D3DXMATRIX worldMatrix, 
											D3DXMATRIX viewMatrix, 
											D3DXMATRIX projectionMatrix,
											D3DXVECTOR3 mEyePos, 
											Light lightVar,											
											ID3D10ShaderResourceView *specularMap,
											ID3D10ShaderResourceView *blendMap,
											ID3D10ShaderResourceView* diffuseMapRV1,
											ID3D10ShaderResourceView* diffuseMapRV2,
											ID3D10ShaderResourceView* diffuseMapRV3,
											float maxHeight,
											int lightType)
{

	Shader::SetShaderParameters(worldMatrix,viewMatrix,projectionMatrix);

	// Set the eye position variable inside the shader
	mEyePosVar->SetRawValue(&mEyePos, 0, sizeof(D3DXVECTOR3));

	// Set the light variable inside the shader
	mLightVar->SetRawValue(&lightVar, 0, sizeof(Light));

	mLightType->SetInt(lightType);

	// Set the diffuse map shader var
	mSpecularMap->SetResource(specularMap);

	// Set the blend map shader var
	mBlendMap->SetResource(blendMap);

	//Set the diffuse map RV shader vars
	mDiffuseMapRV[0]->SetResource(diffuseMapRV1);
	mDiffuseMapRV[1]->SetResource(diffuseMapRV2);
	mDiffuseMapRV[2]->SetResource(diffuseMapRV3);

	//Set the height variables - the first one is 0, the second one is in the middle between them and the third is the max
	mHeights[0]->SetFloat(0.0f);
	mHeights[1]->SetFloat(maxHeight/3.0f);
	mHeights[2]->SetFloat(maxHeight);
}

bool TexShader::InitializeShader(ID3D10Device* device, HWND hwnd, WCHAR* filename){

	HRESULT result;
	ID3D10Blob* errorMessage;
	
	unsigned int numElements;
	D3D10_PASS_DESC passDesc;

	// Initialize the error message.
	errorMessage = 0;

	//Initialize shader flags
	DWORD shaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
	#if defined( DEBUG ) || defined( _DEBUG )
		shaderFlags |= D3D10_SHADER_DEBUG;
		shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
	#endif

	/*Here is where we compile the shader program into an effect. We give it the name of the shader file, 
	the shader version (4.0 in DirectX 10), and the effect to compile the shader into.*/
	// Load the shader in from the file.
	result = D3DX10CreateEffectFromFile(filename, NULL, NULL, "fx_4_0", shaderFlags, 0, 
					    device, NULL, NULL, &mEffect, &errorMessage, NULL);
	if(FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if(errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, filename);
		}
		// If there was  nothing in the error message then it simply could not find the shader file itself.
		else
		{
			MessageBox(hwnd, filename, L"Missing Shader File", MB_OK);
		}

		return false;
	}	

	/*Once the shader code has successfully compiled into an effect we then use that effect to get 
	the technique inside the shader. We will use the technique to draw with the shader from this point forward.*/

	// Get a pointer to the technique inside the shader.
	mTechnique = mEffect->GetTechniqueByName("TextureTechnique");
	if(!mTechnique)
	{
		return false;
	}

	// Now setup the layout of the data that goes into the shader.
	// This setup needs to match the VertexType stucture in the GameObject class and in the shader.
	D3D10_INPUT_ELEMENT_DESC polygonLayout[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D10_APPEND_ALIGNED_ELEMENT, D3D10_INPUT_PER_VERTEX_DATA, 0},
	};
	
	/*Once the layout description has been setup we can get the size of it and then create 
	the input layout using the D3D device.*/

	// Get a count of the elements in the layout.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Get the description of the first pass described in the shader technique.
	mTechnique->GetPassByIndex(0)->GetDesc(&passDesc);

	// Create the input lay9out.
	result = device->CreateInputLayout(polygonLayout, numElements, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, 
					   &mLayout);
	if(FAILED(result))
	{
		return false;
	}

	/*We will also grab pointers to the global matrices that are inside the shader file. 
	This way when we set a matrix from the main app inside the shader easily by just using these pointers.*/

	// Get pointers to the three matrices inside the shader so we can update them from this class.
	mWVPMatrix =	mEffect->GetVariableByName("wvpMatrix")->AsMatrix();
	mWorldMatrix =	mEffect->GetVariableByName("worldMatrix")->AsMatrix();
	mViewMatrix =	mEffect->GetVariableByName("viewMatrix")->AsMatrix();
	mProjectionMatrix = mEffect->GetVariableByName("projectionMatrix")->AsMatrix();

	mEyePosVar		= mEffect->GetVariableByName("gEyePosW");

	mLightVar		= mEffect->GetVariableByName("gLight");
	mLightType		= mEffect->GetVariableByName("gLightType")->AsScalar();

	mDiffuseMap		= mEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
	mSpecularMap	= mEffect->GetVariableByName("gSpecMap")->AsShaderResource();
	mBlendMap		= mEffect->GetVariableByName("gBlendMap")->AsShaderResource();

	mDiffuseMapRV[0]	= mEffect->GetVariableByName("gLayer1")->AsShaderResource();
	mDiffuseMapRV[1]	= mEffect->GetVariableByName("gLayer2")->AsShaderResource();
	mDiffuseMapRV[2]	= mEffect->GetVariableByName("gLayer3")->AsShaderResource();

	mHeights[0]			= mEffect->GetVariableByName("height1")->AsScalar();
	mHeights[1]			= mEffect->GetVariableByName("height2")->AsScalar();
	mHeights[2]			= mEffect->GetVariableByName("height3")->AsScalar();
	return true;
}