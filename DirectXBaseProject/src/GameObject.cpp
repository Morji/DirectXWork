#include "GameObject.h"


void GameObject::setTrans(D3DXMATRIX worldMatrix){
	D3DXMATRIX m;
	D3DXMatrixIdentity(&objMatrix);
	D3DXMatrixScaling(&m, scale.x, scale.y, scale.z);
	objMatrix*=m;
	D3DXMatrixRotationX(&m, theta.x);
	objMatrix*=m;
	D3DXMatrixRotationY(&m, theta.y);
	objMatrix*=m;
	D3DXMatrixRotationZ(&m, theta.z);	
	objMatrix*=m;
	D3DXMatrixTranslation(&m, pos.x, pos.y, pos.z);
	objMatrix*=m;
	objMatrix *= worldMatrix;
}

void GameObject::MoveFacing(float speed){
	pos += Vector3f(objMatrix.m[2][0],0,objMatrix.m[2][2])*speed;
}

void GameObject::MoveStrafe(float speed){
	Vector3f right;
	D3DXVec3TransformNormal(&right, &Vector3f(1,0,0), &objMatrix);//get the objects right vector
	pos += speed*right;
}

//The Initialize function will call the initialization functions for the vertex and index buffers.
bool GameObject::Initialize(ID3D10Device* device){
	bool result;

	md3dDevice = device;

	// Initialize the vertex and index buffer that hold the geometry for the triangle.
	result = SetupArraysAndInitBuffers();
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
	result = LoadTexture(diffuseMapTex, specularMapTex);
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
	result = LoadMultiTexture(specularMapTex, blendMapTex, diffuseMapRV1Tex, diffuseMapRV2Tex, diffuseMapRV3Tex);
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
void GameObject::Render(D3DXMATRIX worldMatrix)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers();
	// Set the transformation matrix for the object
	setTrans(worldMatrix);
}



bool GameObject::SetupArraysAndInitBuffers(){

	//Create vertex array
	VertexNT vertices[] = {
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
	if (!InitializeBuffers(indices, vertices))
		return false;

	return true;
}

//The InitializeBuffers function is where we handle creating the vertex and index buffers. 
bool GameObject::InitializeBuffers(DWORD* indices, VertexNT* vertices){

	D3D10_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D10_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// Set up the description of the vertex buffer.
	vertexBufferDesc.Usage = D3D10_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexNT) * mVertexCount;
	vertexBufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;

	// Now finally create the vertex buffer.
	result = md3dDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &mVB);
	if(FAILED(result))
	{
		return false;
	}

	// Set up the description of the index buffer.
	indexBufferDesc.Usage = D3D10_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * mIndexCount;
	indexBufferDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;

	// Create the index buffer.
	result = md3dDevice->CreateBuffer(&indexBufferDesc, &indexData, &mIB);

	if(FAILED(result)){
		return false;
	}

	stride = sizeof(VertexNT); //set the stride of the buffers to be the size of the vertexNT

	return true;
}



bool GameObject::LoadTexture(WCHAR* diffuseMapTex, WCHAR* specularMapTex){
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
	if (diffuseMapTex != NULL){
		result = diffuseMap->Initialize(md3dDevice, diffuseMapTex);
		if(!result){
			return false;
		}
	}
	if (specularMapTex != NULL){
		result = specularMap->Initialize(md3dDevice, specularMapTex);
		if(!result){
			return false;
		}
	}
	return true;
}

bool GameObject::LoadMultiTexture(WCHAR* specularMapTex, WCHAR* blendMapTex,	WCHAR* diffuseMapRV1Tex,
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
	if(!specularMap){
		return false;
	}
	if (!blendMap){
		return false;
	}

	if (!diffuseMapRV[0]->Initialize(md3dDevice,diffuseMapRV1Tex))
		return false;
	if (!diffuseMapRV[1]->Initialize(md3dDevice,diffuseMapRV2Tex))
		return false;
	if (!diffuseMapRV[2]->Initialize(md3dDevice,diffuseMapRV3Tex))
		return false;

	// Initialize the texture object.
	if (specularMapTex != NULL){
		result = specularMap->Initialize(md3dDevice, specularMapTex);
		if(!result){
			return false;
		}
	}
	if (blendMapTex != NULL){
		result = blendMap->Initialize(md3dDevice, blendMapTex);
		if(!result){
			return false;
		}
	}
	return true;
}

void GameObject::ReleaseTexture(){
	// Release the texture object.
	if (diffuseMap)	{diffuseMap->Shutdown();	delete diffuseMap;	diffuseMap = 0;}
	if (specularMap){specularMap->Shutdown();	delete specularMap; specularMap = 0;}
	if (blendMap)	{blendMap->Shutdown();		delete blendMap;	blendMap = 0;}

	for (int i = 0; i < 3; i++){ 
		if (diffuseMapRV[i]){diffuseMapRV[i]->Shutdown(); delete diffuseMapRV[i]; diffuseMapRV[i] = 0;}
	}
	
}

//The ShutdownBuffers function just releases the vertex buffer and index buffer that were created in the InitializeBuffers function.
void GameObject::ShutdownBuffers(){
	// Release the index buffer.
	ReleaseCOM(mIB);
	// Release the vertex buffer.
	ReleaseCOM(mVB);
}

void GameObject::RenderBuffers(){

	// Set vertex buffer stride and offset.
	offset = 0;
    
	// Set the vertex buffer to active in the input assembler so it can be rendered.
	md3dDevice->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	md3dDevice->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	md3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
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