#include "Grid.h"


Grid::Grid(void)
{
}


Grid::~Grid(void)
{
	Shutdown();
}

bool Grid::GenerateGrid(ID3D10Device* device, int width, int depth){

	mVertexCount = (width*depth);
	vertices = new VertexC[mVertexCount];

	float dx = CELLSPACING;
	float halfWidth = (width-1)*dx*0.5f;
	float halfDepth = (depth-1)*dx*0.5f;

	for(DWORD i = 0; i < width; ++i){
		float z = halfDepth - i*dx;

		for(DWORD j = 0; j < depth; ++j){
			float x = -halfWidth + j*dx;
			// Graph of this function looks like a mountain range.
			float y = GetHeight(x,z);
			//float y = 0.0f;
			vertices[i*depth+j].pos = D3DXVECTOR3(x, y, z);
			// Color the vertex based on its height.
			if( y < -10.0f )
				vertices[i*depth+j].color = BEACH_SAND;
			else if( y < 5.0f )
				vertices[i*depth+j].color = LIGHT_YELLOW_GREEN;
			else if( y < 12.0f )
				vertices[i*depth+j].color = DARK_YELLOW_GREEN;
			else if( y < 20.0f )
				vertices[i*depth+j].color = DARKBROWN;
			else
				vertices[i*depth+j].color = WHITE;
		}
	}
	
	// Iterate over each quad and compute indices.
	mIndexCount = ((width-1)*(depth-1)*6);
	indices = new DWORD[mIndexCount];
	int k = 0;
	for(DWORD i = 0; i < width-1; ++i){
		for(DWORD j = 0; j < depth-1; ++j){
			indices[k] = i*depth+j;
			indices[k+1] = i*depth+j+1;
			indices[k+2] = (i+1)*depth+j;

			indices[k+3] = (i+1)*depth+j;
			indices[k+4] = i*depth+j+1;
			indices[k+5] = (i+1)*depth+j+1;
			k += 6; // next quad
		}
	}

	//initialize the buffers with the index and vertex data
	if (!InitializeBuffers(device, indices, vertices))
		return false;

	return true;
}

float Grid::GetHeight(float x, float z)const{
	return 0.3f*( z*sinf(0.1f*x) + x*cosf(0.1f*z) );
}

//The InitializeBuffers function is where we handle creating the vertex and index buffers. 
bool Grid::InitializeBuffers(ID3D10Device* device, DWORD* indices,  VertexC* vertices){

	D3D10_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D10_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	// Set up the description of the vertex buffer.
	vertexBufferDesc.Usage = D3D10_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexC) * mVertexCount;
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

	stride = sizeof(VertexC);

	return true;
	
}