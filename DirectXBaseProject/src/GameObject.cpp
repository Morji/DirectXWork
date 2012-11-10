#include "GameObject.h"


void GameObject::setTrans(D3DXMATRIX worldMatrix){
	D3DXMATRIX m;
	D3DXMatrixIdentity(&objMatrix);
	D3DXMatrixScaling(&m, scale.x, scale.y, scale.z);
	objMatrix*=m;
	D3DXMatrixRotationY(&m, theta.y);
	objMatrix*=m;
	D3DXMatrixTranslation(&m, pos.x, pos.y, pos.z);
	objMatrix*=m;
	objMatrix *= worldMatrix;
}

//The Initialize function will call the initialization functions for the vertex and index buffers.
bool GameObject::Initialize(ID3D10Device* device){
	bool result;

	// Initialize the vertex and index buffer that hold the geometry for the triangle.
	result = SetupArraysAndInitBuffers(device);
	if(!result)	{
		return false;
	}

	return true;
}

//The Initialize function will call the initialization functions for the vertex and index buffers.
bool GameObject::InitializeWithTexture(ID3D10Device* device, WCHAR* diffuseMapTex, WCHAR* specularMapTex){
	bool result;

	// Initialize the vertex and index buffer that hold the geometry for the triangle.
	result = Initialize(device);
	if(!result)	{
		return false;
	}

	// Load the texture for this model.
	result = LoadTexture(device, diffuseMapTex, specularMapTex);
	if(!result){
		return false;
	}
	return true;
}

//The Initialize function will call the initialization functions for the vertex and index buffers.
bool GameObject::InitializeWithMultiTexture(ID3D10Device* device, WCHAR* specularMapTex, WCHAR* blendMapTex, WCHAR* diffuseMapRV1Tex,
																											WCHAR* diffuseMapRV2Tex,
																											WCHAR* diffuseMapRV3Tex)
{
	bool result;

	// Initialize the vertex and index buffer that hold the geometry for the triangle.
	result = Initialize(device);
	if(!result)	{
		return false;
	}

	// Load the texture for this model.
	result = LoadMultiTexture(device, specularMapTex, blendMapTex, diffuseMapRV1Tex, diffuseMapRV2Tex, diffuseMapRV3Tex);
	if(!result){
		return false;
	}
	return true;
}

//The Shutdown function will call the shutdown functions for the vertex and index buffers.
void GameObject::Shutdown(){
	// Release the model texture.
	ReleaseTexture();

	// Release the vertex and index buffers.
	ShutdownBuffers();
}

//Render is called from the GraphicsClass::Render function. This function calls RenderBuffers to put the vertex and index buffers on the graphics pipeline so the color shader will be able to render them.
void GameObject::Render(ID3D10Device* device, D3DXMATRIX worldMatrix)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(device);
	// Set the transformation matrix for the object
	setTrans(worldMatrix);
}



bool GameObject::SetupArraysAndInitBuffers(ID3D10Device* device){

	//Create vertex array
	Vertex vertices[] = {
		{D3DXVECTOR3(-1.0f, -1.0f, 0.0f), Vector2f(0.0f, 1.0f)},			
		{D3DXVECTOR3(-1.0f, +1.0f, 0.0f), Vector2f(0.5f, 0.0f)},
		{D3DXVECTOR3(+1.0f, +1.0f, 0.0f), Vector2f(1.0f, 1.0f)}
	};

	// Create index array
	DWORD indices[] = {
		 0,1,2
	};

	// Set the number of vertices in the vertex array.
	mVertexCount = sizeof(vertices)/sizeof(vertices[0]);
	// Set the number of indices in the index array.
	mIndexCount = sizeof(indices)/sizeof(indices[0]);

	//initialize the buffers
	if (!InitializeBuffers(device, indices, vertices))
		return false;

	return true;
}

//The InitializeBuffers function is where we handle creating the vertex and index buffers. 
bool GameObject::InitializeBuffers(ID3D10Device* device, DWORD* indices,  Vertex* vertices){

	D3D10_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D10_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// Set up the description of the vertex buffer.
	vertexBufferDesc.Usage = D3D10_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * mVertexCount;
	vertexBufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;

	// Now finally create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &mVB);
	if(FAILED(result))
	{
		return false;
	}

	// Set up the description of the index buffer.
	indexBufferDesc.Usage = D3D10_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * mIndexCount;
	indexBufferDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &mIB);

	if(FAILED(result))
	{
		return false;
	}

	return true;
}



bool GameObject::LoadTexture(ID3D10Device* device, WCHAR* diffuseMapTex, WCHAR* specularMapTex){
	bool result;

	// Create the texture object.
	diffuseMap = new TextureLoader;
	specularMap = new TextureLoader;
	if(!diffuseMap)	{
		return false;
	}
	if (!specularMap){
		return false;
	}

	// Initialize the texture object.
	result = diffuseMap->Initialize(device, diffuseMapTex);
	if(!result){
		return false;
	}
	result = specularMap->Initialize(device, specularMapTex);
	if(!result){
		return false;
	}
	return true;
}

bool GameObject::LoadMultiTexture(ID3D10Device* device, WCHAR* specularMapTex, WCHAR* blendMapTex,	WCHAR* diffuseMapRV1Tex,
																									WCHAR* diffuseMapRV2Tex,
																									WCHAR* diffuseMapRV3Tex){
	bool result;

	// Create the texture object.
	specularMap = new TextureLoader;
	blendMap = new TextureLoader;
	for (int i = 0; i < 3; i++){
		diffuseMapRV[i] = new TextureLoader;

		if (!diffuseMapRV[i])
			return false;
	}
	if(!specularMap)	{
		return false;
	}
	if (!blendMap){
		return false;
	}

	if (!diffuseMapRV[0]->Initialize(device,diffuseMapRV1Tex))
		return false;
	if (!diffuseMapRV[1]->Initialize(device,diffuseMapRV1Tex))
		return false;
	if (!diffuseMapRV[2]->Initialize(device,diffuseMapRV1Tex))
		return false;

	// Initialize the texture object.
	result = specularMap->Initialize(device, specularMapTex);
	if(!result){
		return false;
	}
	result = blendMap->Initialize(device, blendMapTex);
	if(!result){
		return false;
	}
	return true;
}

void GameObject::ReleaseTexture(){
	// Release the texture object.
	if(diffuseMap){
		diffuseMap->Shutdown();
		delete diffuseMap;
		diffuseMap = 0;
	}
	if (specularMap){
		specularMap->Shutdown();
		delete specularMap;
		specularMap = 0;
	}
}

//The ShutdownBuffers function just releases the vertex buffer and index buffer that were created in the InitializeBuffers function.
void GameObject::ShutdownBuffers(){
	// Release the index buffer.
	if(mIB)
	{
		mIB->Release();
		mIB = 0;
	}

	// Release the vertex buffer.
	if(mVB)
	{
		mVB->Release();
		mVB = 0;
	}
}

void GameObject::RenderBuffers(ID3D10Device* device){
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(Vertex); 
	offset = 0;
    
	// Set the vertex buffer to active in the input assembler so it can be rendered.
	device->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	device->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

////GETTERS
//GetIndexCount returns the number of indexes in the model. The shader will need this information to draw this model.
int GameObject::GetIndexCount(){
	return mIndexCount;
}

ID3D10ShaderResourceView* GameObject::GetDiffuseTexture(){
	return diffuseMap->GetTexture();
}

ID3D10ShaderResourceView* GameObject::GetSpecularTexture(){
	return specularMap->GetTexture();
}

ID3D10ShaderResourceView* GameObject::GetBlendTexture(){
	return blendMap->GetTexture();
}

ID3D10ShaderResourceView* GameObject::GetDiffuseMap(int rvWhich){
	return diffuseMapRV[rvWhich]->GetTexture();
}