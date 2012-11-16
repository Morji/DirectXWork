#include "Shader.h"


Shader::Shader(void)
{
	mEffect = 0;
	mTechnique = 0;
	mLayout = 0;

	mWorldMatrix = 0;
	mViewMatrix = 0;
	mProjectionMatrix = 0;
}

Shader::~Shader(void)
{
}

/*The Initialize function will call the initialization function for the shader. 
We pass in the name of the HLSL shader file*/
bool Shader::Initialize(ID3D10Device* device, HWND hwnd)
{
	bool result;

	// Initialize the shader that will be used to draw the triangle.
	result = InitializeShader(device, hwnd, L"assets/color2.fx");
	if(!result){
		return false;
	}

	return true;
}

//The Shutdown function will call the shutdown of the shader.
void Shader::Shutdown(){
	// Shutdown the shader effect.
	ShutdownShader();
}

/*Render will first set the parameters inside the shader using the SetShaderParameters function. 
Once the parameters are set it then calls RenderShader to draw using the HLSL shader.*/
void Shader::Render(ID3D10Device* device, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix)
{
	// Set the shader parameters that it will use for rendering.
	SetShaderParameters(worldMatrix, viewMatrix, projectionMatrix);

	// Now render the prepared buffers with the shader.
	RenderShader(device, indexCount);
}

 /* This function is what actually loads the shader file and makes it usable to DirectX and the GPU. 
 You will also see the setup of the layout and how the vertex buffer data is going to look
 on the graphics pipeline in the GPU. The layout will need the match the Vertex struct in GameObject.h
 as well as the one defined in the shader.fx file*/
bool Shader::InitializeShader(ID3D10Device* device, HWND hwnd, WCHAR* filename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	D3D10_INPUT_ELEMENT_DESC polygonLayout[2];
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
	mTechnique = mEffect->GetTechniqueByName("ColorTechnique");
	if(!mTechnique)
	{
		return false;
	}

	// Now setup the layout of the data that goes into the shader.
	// This setup needs to match the VertexType stucture in the GameObject class and in the shader.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D10_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	/*Once the layout description has been setup we can get the size of it and then create 
	the input layout using the D3D device.*/

	// Get a count of the elements in the layout.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Get the description of the first pass described in the shader technique.
	mTechnique->GetPassByIndex(0)->GetDesc(&passDesc);

	// Create the input lay9out.
	result = device->CreateInputLayout(polygonLayout, numElements, passDesc.pIAInputSignature, passDesc.IAInputSignatureSize, 
					   &mLayout);
	if(FAILED(result)){
		return false;
	}

	/*We will also grab pointers to the global matrices that are inside the shader file. 
	This way when we set a matrix from the main app inside the shader easily by just using these pointers.*/

	// Get pointers to the three matrices inside the shader so we can update them from this class.
	mWorldMatrix = mEffect->GetVariableByName("worldMatrix")->AsMatrix();
	mViewMatrix = mEffect->GetVariableByName("viewMatrix")->AsMatrix();
	mProjectionMatrix = mEffect->GetVariableByName("projectionMatrix")->AsMatrix();

	return true;
}

//ShutdownShader releases the interfaces and pointers that were setup in the InitializeShader function.

void Shader::ShutdownShader(){
	// Release the pointers to the matrices inside the shader.
	mWorldMatrix = 0;
	mViewMatrix = 0;
	mProjectionMatrix = 0;

	// Release the pointer to the shader layout.
	ReleaseCOM(mLayout);

	// Release the pointer to the shader technique.
	mTechnique = 0;

	// Release the pointer to the shader.
	ReleaseCOM(mEffect);
}

/*The OutputShaderErrorMessage writes out error messages that are generating when compiling 
either vertex shaders or pixel shaders.*/
void Shader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	std::ofstream fout;

	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();

	// Open a file to write the error message to.
	fout.open("shader-error.txt");

	// Write out the error message.
	for(i=0; i<bufferSize; i++){
		fout << compileErrors[i];
	}

	// Close the file.
	fout.close();

	// Release the error message.
	ReleaseCOM(errorMessage);

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);
}

/*The SetShaderVariables function exists to make setting the global variables in the shader easier. 
The matrices used in this function are created inside the main app, after which
this function is called to send them from there into the shader during the Render function call.*/
void Shader::SetShaderParameters(D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix)
{
	// Set the wvp matrix inside the shader
	D3DXMATRIX mWVP = worldMatrix*viewMatrix*projectionMatrix;
	mWVPMatrix->SetMatrix((float*)&mWVP);

	// Set the world matrix variable inside the shader.
	mWorldMatrix->SetMatrix((float*)&worldMatrix);

	// Set the view matrix variable inside the shader.
	mViewMatrix->SetMatrix((float*)&viewMatrix);

	// Set the projection matrix variable inside the shader.
	mProjectionMatrix->SetMatrix((float*)&projectionMatrix);
}

//RenderShader will invoke the HLSL shader program through the technique pointer.
void Shader::RenderShader(ID3D10Device* device, int indexCount)
{
	D3D10_TECHNIQUE_DESC techniqueDesc;

	// Set the input layout.
	device->IASetInputLayout(mLayout);

	// Get the description structure of the technique from inside the shader so it can be used for rendering.
	mTechnique->GetDesc(&techniqueDesc);

	// Go through each pass in the technique (should be just one currently) and render the triangles.
	for(unsigned int i = 0; i < techniqueDesc.Passes; i++)
	{
		mTechnique->GetPassByIndex(i)->Apply(0);
		device->DrawIndexed(indexCount, 0, 0);
	}
}