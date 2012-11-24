#include "ModelObject.h"



ModelObject::ModelObject(void){
}


ModelObject::~ModelObject(void){	
	Shutdown();
	if (modelLoader){
		delete modelLoader;
		modelLoader = nullptr;
	}
}

bool ModelObject::LoadModelFromFBX(const char* filename){
	modelLoader = new ModelLoader();
	
	if (!modelLoader->LoadModel(filename)){
		return false;
	}

	VertexNT *vertices = modelLoader->GetVertexData();
	DWORD	*indices  = modelLoader->GetIndexData();	

	// Set the number of vertices in the vertex array.
	mVertexCount = modelLoader->GetVertexCount();
	// Set the number of indices in the index array.
	mIndexCount = modelLoader->GetIndexCount();

	if (!InitializeBuffers(indices,vertices))
		return false;

	//Do not do any other pointer cleanup here - the model loader takes care of that 
	if (modelLoader){
		delete modelLoader;
		modelLoader = nullptr;
	}

	return true;
}