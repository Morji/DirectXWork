#include "Isohedron.h"

Isohedron::Isohedron(void){
	explosiveForce = 0;
	explosiveRate = 0;
	isExploding = false;
	isResetting = false;
}


Isohedron::~Isohedron(void){
	//Shutdown();
}

void Isohedron::Render(D3DXMATRIX worldMatrix){
	GameObject::Render(worldMatrix);

	if (isExploding){
		Explode();
	}
}

void Isohedron::Reset(){
	isResetting = true;
}

float Isohedron::GetExplosiveForce(){
	return explosiveForce;
}

bool Isohedron::IsExploding(){
	return isExploding;
}

void Isohedron::StartExplosion(float rate){
	explosiveRate = rate;
	isResetting = false;
	isExploding = true;
}

void Isohedron::StopExplosion(){
	explosiveForce = 0;
	explosiveRate = 0;
	isResetting = false;
	isExploding = false;
}

void Isohedron::Explode(){
	if (isResetting){
		if (explosiveForce > 0)
			explosiveForce -= explosiveRate;
		else if (explosiveForce < 0)
			explosiveForce = 0;
	}
	else{
		explosiveForce += explosiveRate;
		if (explosiveForce >= EXPLOSION_LIMIT){
			StopExplosion();
		}
	}
}

bool Isohedron::SetupArraysAndInitBuffers(){

	const float X = 0.525731f;
	const float Z = 0.850651f;
	const float nX = 0.417775f;
	const float nZ = 0.675973f;
	

	// Load the vertex array with data.
	VertexNC vertices[12];

	// 12 unique vertices
	D3DXVECTOR3 pos[12] =
	{
		D3DXVECTOR3(-X, 0.0f, Z), D3DXVECTOR3(X, 0.0f, Z),
		D3DXVECTOR3(-X, 0.0f, -Z), D3DXVECTOR3(X, 0.0f, -Z),
		D3DXVECTOR3(0.0f, Z, X), D3DXVECTOR3(0.0f, Z, -X),
		D3DXVECTOR3(0.0f, -Z, X), D3DXVECTOR3(0.0f, -Z, -X),
		D3DXVECTOR3(Z, X, 0.0f), D3DXVECTOR3(-Z, X, 0.0f),
		D3DXVECTOR3(Z, -X, 0.0f), D3DXVECTOR3(-Z, -X, 0.0f)
	};

	//cheat sheet for the normals
	D3DXVECTOR3 normals[12] = {
		D3DXVECTOR3(-nX, 0.0f, nX),D3DXVECTOR3(nX, 0.0f, nZ),
		D3DXVECTOR3(-nX, 0.0f, -nZ),D3DXVECTOR3(nX, 0.0f, -nZ),
		D3DXVECTOR3(0.0f, nZ, nX),D3DXVECTOR3(0.0f, nZ, -nX),
		D3DXVECTOR3(0.0f, -nZ, nX), D3DXVECTOR3(0.0f, -nZ, -nX),
		D3DXVECTOR3(nZ, nX, 0.0f), D3DXVECTOR3(-nZ, nX, 0.0f),
		D3DXVECTOR3(nZ, -nX, 0.0f), D3DXVECTOR3(-nZ, -nX, 0.0f)
	};

	for (int i = 0; i < 12; i++){
		vertices[i].pos = pos[i];
		vertices[i].normal = normals[i];
		vertices[i].color = Color(RandF(),RandF(),RandF(),RandF());
	}


	// 20 triangles
	DWORD indicesArray[60] =
	{
		1,4,0, 4,9,0, 4,5,9, 8,5,4, 1,8,4,
		1,10,8, 10,3,8, 8,3,5, 3,2,5, 3,7,2,
		3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0,
		10,1,6, 11,0,9, 2,11,9, 5,2,9, 11,2,7
	};

	// Set the number of vertices in the vertex array.
	mVertexCount = sizeof(vertices)/sizeof(vertices[0]);
	// Set the number of indices in the index array.
	mIndexCount = sizeof(indicesArray)/sizeof(indicesArray[0]);

	//initialize the buffers with the index and vertex data
	if (!InitializeBuffers(indicesArray, vertices))
		return false;

	return true;
}

//The InitializeBuffers function is where we handle creating the vertex and index buffers. 
bool Isohedron::InitializeBuffers(DWORD* indices, VertexNC* vertices){

	D3D10_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D10_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// Set up the description of the vertex buffer.
	vertexBufferDesc.Usage = D3D10_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexNC) * mVertexCount;
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

	stride = sizeof(VertexNC); //set the stride of the buffers to be the size of the vertexNT

	return true;
}