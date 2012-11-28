#include "Grid.h"


Grid::Grid(void)
{
	maxHeight = 0.0f;
	heightData = nullptr;
	vertices = nullptr;
	indices = nullptr;
	texOffset = Vector2f(0,0);
}


Grid::~Grid(void)
{
	Shutdown();
	if (vertices){
		delete [] vertices;
		vertices = nullptr;		
	}
	if (indices){
		delete [] indices;
		indices = nullptr;		
	}
	if (heightData){
		delete [] heightData;
		heightData = nullptr;
	}
}

bool Grid::GenerateGridFromTGA(char* filename){
	TerrainLoader *terrainLoader = new TerrainLoader();

	if (!terrainLoader->LoadTerrain(filename)){
		return false;
	}

	gridWidth = terrainLoader->GetWidth();
	gridDepth = terrainLoader->GetDepth();

	mVertexCount = (gridWidth*gridDepth);
	vertices = new VertexNT[mVertexCount];

	heightData = new float[gridWidth*gridDepth];

	float dx = CELLSPACING;
	float halfWidth = (gridWidth-1)*dx*0.5f;
	float halfDepth = (gridDepth-1)*dx*0.5f;

	for(DWORD i = 0; i < gridWidth; ++i){
		float z = halfDepth - i*dx;

		for(DWORD j = 0; j < gridDepth; ++j){
			float x = -halfWidth + j*dx;
			// Graph of this function looks like a mountain range.
			float y = terrainLoader->GetHeight(i,j)*HEIGHT_FACTOR;
			heightData[i*gridDepth+j] = y;//place in height data array
			vertices[i*gridDepth+j].pos = Vector3f(x, y, z);
			if (y > maxHeight){
				maxHeight = y;
			}
		}
	}

	ComputeNormals();
	ComputeTextureCoords();
	
	// Iterate over each quad and compute indices.
	mIndexCount = ((gridWidth-1)*(gridDepth-1)*6);
	indices = new DWORD[mIndexCount];
	int k = 0;
	for(DWORD i = 0; i < gridWidth-1; ++i){
		for(DWORD j = 0; j < gridDepth-1; ++j){
			// Upper left.

			indices[k] = i*gridDepth+j;
			indices[k+1] = i*gridDepth+j+1;
			indices[k+2] = (i+1)*gridDepth+j;

			indices[k+3] = (i+1)*gridDepth+j;
			indices[k+4] = i*gridDepth+j+1;
			indices[k+5] = (i+1)*gridDepth+j+1;
			k += 6; // next quad
		}
	}

	//initialize the buffers with the index and vertex data
	if (!InitializeBuffers(indices, vertices))
		return false;

	delete terrainLoader;
	terrainLoader = nullptr;

	return true;
}

///Generate a a flat grid given it's size and also how often to repeat(tile) the texture
bool Grid::GenerateGrid(int width, int depth,const int texRepeat){

	gridWidth = width;
	gridDepth = depth;

	mVertexCount = (gridWidth*gridDepth);
	vertices = new VertexNT[mVertexCount];

	float dx = CELLSPACING;
	float halfWidth = (width-1)*dx*0.5f;
	float halfDepth = (depth-1)*dx*0.5f;	

	for(DWORD i = 0; i < width; ++i){
		float z = halfDepth - i*dx;

		for(DWORD j = 0; j < depth; ++j){
			float x = -halfWidth + j*dx;
			float y = 0.0f;
			vertices[i*depth+j].pos = D3DXVECTOR3(x, y, z);
			vertices[i*depth+j].normal = D3DXVECTOR3(0,1,0);
		}
	}
	
	ComputeTextureCoords(texRepeat);

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
	if (!InitializeBuffers(indices, vertices))
		return false;

	return true;
}

//compute the vertex normals
void Grid::ComputeNormals()const{

	for (int i = 0; i < gridWidth; i++){
		for (int j = 0; j < gridDepth; j++){
			Vector3f v1,v2,v3,v4,v12,v23,v34,v41,v;
			v1 = v2 = v3 = v4 = v12 = v23 = v34 = v41 = v = Vector3f(0.0f,0.0f,0.0f);
			//grab 2 vectors for this
			if (j != gridWidth - 1){
				v1 = Vector3f(vertices[i*gridDepth+j+1].pos-vertices[i*gridDepth+j].pos);
			}
			if (i != gridWidth - 1){
				v2 = Vector3f(vertices[(i+1)*gridDepth+j].pos-vertices[i*gridDepth+j].pos);
			}
			if (j > 0){
				v3 = Vector3f(vertices[i*gridDepth+j-1].pos-vertices[i*gridDepth+j].pos);
			}
			if (i > 0){
				v4 = Vector3f(vertices[(i-1)*gridDepth+j].pos-vertices[i*gridDepth+j].pos);
			}

			D3DXVec3Cross(&v12,&v1,&v2);
			D3DXVec3Normalize(&v12,&v12);

			D3DXVec3Cross(&v23,&v2,&v3);
			D3DXVec3Normalize(&v23,&v23);
			
			D3DXVec3Cross(&v34,&v3,&v4);
			D3DXVec3Normalize(&v34,&v34);
			
			D3DXVec3Cross(&v41,&v4,&v1);
			D3DXVec3Normalize(&v41,&v41);

			
			if (D3DXVec3Length(&v12) > 0.0f)
				v = v + v12;

			if (D3DXVec3Length(&v23) > 0.0f)
				v = v + v23;
			
			if (D3DXVec3Length(&v34) > 0.0f)
				v = v + v34;

			if (D3DXVec3Length(&v41) > 0.0f)
				v = v + v41;

			D3DXVec3Normalize(&v,&v);

			vertices[i*gridDepth+j].normal = v;			
		}
	}
}

void Grid::ComputeTextureCoords(const int repeatAmount)const{

	float incrementValue;

	// Calculate how much to increment the texture coordinates by.
	incrementValue = (float)repeatAmount / (float)gridDepth;

	float widthRepeat = (float)(gridWidth/repeatAmount);
	float depthRepeat = (float)(gridDepth/repeatAmount);

	// Loop through the entire height map and calculate the tu and tv texture coordinates for each vertex.
	for(int i=0; i<gridWidth; i++){
		for(int j=0; j<gridDepth; j++){
			vertices[i*gridDepth+j].texC = Vector2f(i / widthRepeat, j / depthRepeat);
		}
	}
}

//The InitializeBuffers function is where we handle creating the vertex and index buffers. 
bool Grid::InitializeBuffers(DWORD* indices,  VertexNT* vertices){

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
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * mIndexCount;
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
	stride = sizeof(VertexNT);
	return true;	
}

void Grid::AnimateUV(float dt){
	// Animate water texture as a function of time in the update function.
	texOffset.y += 0.1f*dt;
	texOffset.x = 0.25f*sinf(4.0f*texOffset.y);

	// Scale texture coordinates by 5 units to map [0,1]-->[0,5]
	// so that the texture repeats five times in each direction.
	D3DXMATRIX S;
	D3DXMatrixScaling(&S, 5.0f, 5.0f, 1.0f);
	// Translate the texture.
	D3DXMATRIX T;
	D3DXMatrixTranslation(&T, texOffset.x, texOffset.y, 0.0f);
	// Scale and translate the texture.
	mTexMatrix = S*T;
}

float Grid::GetMaxHeight(){
	return maxHeight;
}

float Grid::GetHeight(float x, float z){
	// Transform from terrain local space to “cell” space.
	float c = (x + 0.5f*gridWidth) / CELLSPACING;
	float d = (z - 0.5f*gridDepth) / -CELLSPACING;

	//check if point is within the grid
	if (c < 0 || c > gridWidth || d < 0 || d > gridDepth){
		return 0.0f;
	}

	// Get the row and column we are in.
	int row = (int)floorf(d);//z
	int col = (int)floorf(c);//x

	// Grab the heights of the cell we are in.
	// A*--*B
	// | /|
	// |/ |
	// C*--*D
	float A = heightData[row*gridWidth + col];
	float B = heightData[row*gridWidth + col + 1];
	float C = heightData[(row+1)*gridWidth + col];
	float D = heightData[(row+1)*gridWidth + col + 1];

	// Find out which of the two triangles of the cell we are in
	// if s + t <= 1 we are in the upper triangle
	// else we are in the lower one
	float s = c - (float)col;
	float t = d - (float)row;

	// If upper triangle ABC.
	if( s + t <= 1.0f){
		float uy = B - A;
		float vy = C - A;
		return A + s*uy + t*vy;
	}
	// lower triangle DCB.
	else{ 	
		float uy = C - D;
		float vy = B - D;
		return D + (1.0f-s)*uy + (1.0f-t)*vy;
	}
}