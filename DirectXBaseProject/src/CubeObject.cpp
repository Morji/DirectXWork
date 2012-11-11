#include "CubeObject.h"


bool CubeObject::SetupArraysAndInitBuffers(ID3D10Device* device){

	// Load the vertex array with data.
	VertexNT vertices[] = {
		//POS							//NORMAL					   //TEX
		//back
		{Vector3f(-1.0f, -1.0f, -1.0f),	Vector3f(-1.0f, -1.0f, -1.0f), Vector2f(0.0f, 1.0f)}, // Bottom left.	
		{Vector3f(-1.0f, +1.0f, -1.0f), Vector3f(-1.0f, +1.0f, -1.0f), Vector2f(0.0f, 0.0f)}, // Top Left
		{Vector3f(+1.0f, +1.0f, -1.0f), Vector3f(+1.0f, +1.0f, -1.0f), Vector2f(1.0f, 0.0f)}, // Top Right
		{Vector3f(+1.0f, -1.0f, -1.0f), Vector3f(+1.0f, -1.0f, -1.0f), Vector2f(1.0f, 1.0f)}, // Bottom Right

		//front
		{Vector3f(-1.0f, -1.0f, 1.0f), Vector3f(-1.0f,- 1.0f, 1.0f), Vector2f(1.0f, 1.0f)}, // Bottom left.	
		{Vector3f(-1.0f, +1.0f, 1.0f), Vector3f(-1.0f, +1.0f, 1.0f), Vector2f(0.0f, 1.0f)}, // Top Left
		{Vector3f(+1.0f, +1.0f, 1.0f), Vector3f(+1.0f, +1.0f, 1.0f), Vector2f(0.0f, 0.0f)}, // Top Right
		{Vector3f(+1.0f, -1.0f, 1.0f), Vector3f(+1.0f, -1.0f, 1.0f), Vector2f(1.0f, 0.0f)}, // Bottom Right
	};


	DWORD indicesArray[] ={
		// Back face
        0,1,2,
        0,2,3,
        // Front face
        4,6,5,
        4,7,6,
        // Left face
        4,5,1,
        4,1,0,
        // Right face
        3,2,6,
        3,6,7,
        // Top face
        1,5,6,
        1,6,2,
        // Bottom face
        4,0,3,
        4,3,7
	};

	// Set the number of vertices in the vertex array.
	mVertexCount = sizeof(vertices)/sizeof(vertices[0]);
	// Set the number of indices in the index array.
	mIndexCount = sizeof(indicesArray)/sizeof(indicesArray[0]);

	//initialize the buffers with the index and vertex data
	if (!InitializeBuffers(device, indicesArray, vertices))
		return false;

	return true;
}