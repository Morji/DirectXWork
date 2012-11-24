#include "ModelLoader.h"


ModelLoader::ModelLoader(void){
	numNodes = 0;
}


ModelLoader::~ModelLoader(void){
	while (!vertexData.empty())
		vertexData.pop_back();
	vertexData.clear();

	while (!indexData.empty())
		indexData.pop_back();
	indexData.clear();
}

bool ModelLoader::LoadModel(const char* filename){

	std::cout << "Starting model loader..." << std::endl;

	// Initialize the sdk manager. This object handles all our memory management.
    FbxManager* lSdkManager = FbxManager::Create();
    
    // Create the io settings object.
    FbxIOSettings *ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
    lSdkManager->SetIOSettings(ios);

    // Create an importer using our sdk manager.
    FbxImporter* lImporter = FbxImporter::Create(lSdkManager,"");
    
    // Use the first argument as the filename for the importer.
    if(!lImporter->Initialize(filename, -1, lSdkManager->GetIOSettings())) {
        return false;
    }
    
    // Create a new scene so it can be populated by the imported file.
    FbxScene* lScene = FbxScene::Create(lSdkManager,"myScene");

    // Import the contents of the file into the scene.
	std::cout << "Importing scene..." << std::endl;
    lImporter->Import(lScene);
	std::cout << "Scene imported" << std::endl;
    // The file has been imported; we can get rid of the importer.
    lImporter->Destroy();
    
    // Get the nodes of the scene and their attributes recursively.
    // Note that we are not getting the root node, because it shouldw
    // not contain any attributes.
    FbxNode* lRootNode = lScene->GetRootNode();
	
	std::cout << "Loading Model name: " << filename << std::endl;	

    if(lRootNode){
        for(int i = 0; i < lRootNode->GetChildCount(); i++){
            if (!GetNodeData(lRootNode->GetChild(i)))
				return false;
		}
    }

    // Destroy the sdk manager and all other objects it was handling.
    lSdkManager->Destroy();

	std::cout << "Number of fbx nodes: " << numNodes << std::endl;
	std::cout << "Finished model loading" << std::endl;

	return true;
}

bool ModelLoader::GetNodeData(FbxNode *pNode){
	numNodes++;

	if (pNode->GetMesh() != NULL){
		if (!GetMeshData(pNode->GetMesh()))
			return false;
	}

	

	// Recursively get the children nodes.
    for(int j = 0; j < pNode->GetChildCount(); j++){
        if (!GetNodeData(pNode->GetChild(j)))
			return false;
	}
	return true;
}

bool ModelLoader::GetMeshData(FbxMesh *pMesh){

	std::cout << "Mesh detected " << std::endl;
	std::cout << "Polygon count " << pMesh->GetPolygonCount() << std::endl;
	fbxDouble3 translation = pMesh->GetNode()->LclTranslation.Get();
    fbxDouble3 rotation = pMesh->GetNode()->LclRotation.Get();
    fbxDouble3 scaling = pMesh->GetNode()->LclScaling.Get();

	std::cout << "Translation: " << translation[0] << "," << translation[1] << "," << translation[2] << std::endl;
	std::cout << "Rotation: " << rotation[0] << "," << rotation[1] << "," << rotation[2] << std::endl;
	std::cout << "Scaling: " << scaling[0] << "," << scaling[1] << "," << scaling[2] << std::endl;

	//Get Vertex Data
	if (!PopulateVertexData(pMesh))
		return false;

	//Get Index Data
	if (!PopulateIndexData(pMesh))
		return false;

	return true;
}

bool ModelLoader::PopulateVertexData(FbxMesh *pMesh){
	
	//Each index of this array corresponds to a vertex in the mesh
	FbxVector4* vertexarray = pMesh->GetControlPoints();
	int vertexcount = pMesh->GetControlPointsCount();
	int polygonCount = pMesh->GetPolygonCount();

	//std::cout << "Loading " << pMesh->GetTextureUVCount()<< " texture UVs" << std::endl;

	std::cout << "Loading " << vertexcount << " vertices" << std::endl;
	//Now stores each vertex in my array
	for (int i = 0; i < vertexcount; ++i){
		VertexNT vert;
		//Get coordinates
		vert.pos.x = vertexarray[i].mData[0];
		vert.pos.y = vertexarray[i].mData[1];
		vert.pos.z = vertexarray[i].mData[2];

		vert.texC = Vector2f(0,0);

		vertexData.push_back(vert);
	}

	std::cout << "Loading normals and texture coordinates" << std::endl;

	//Get normals
	// For each polygon in the input mesh
	for (int polygon = 0; polygon < pMesh->GetPolygonCount(); polygon++) {
		// For each vertex in the polygon
		for (unsigned polygonVertex = 0; polygonVertex < 3; polygonVertex++) {
			int fbxCornerIndex = pMesh->GetPolygonVertex(polygon, polygonVertex);
			
			// Get vertex position
			//FbxVector4 fbxVertex = fbxControlPoints[fbxCornerIndex];
			// Get normal
			KFbxVector4 fbxNormal;
			pMesh->GetPolygonVertexNormal(polygon, polygonVertex, fbxNormal);
			fbxNormal.Normalize();
			vertexData[fbxCornerIndex].normal = Vector3f(fbxNormal.mData[0],fbxNormal.mData[1],fbxNormal.mData[2]);

			// Get texture coordinate
			FbxVector2 fbxUV = KFbxVector2(0.0, 0.0);
			FbxLayerElementUV* fbxLayerUV = pMesh->GetLayer(0)->GetUVs();
			if (fbxLayerUV) {
				int UVIndex = 0;
				switch (fbxLayerUV->GetMappingMode()) {
					case KFbxLayerElement::eByControlPoint:
						UVIndex = fbxCornerIndex;
						break;

					case KFbxLayerElement::eByPolygonVertex:
						UVIndex = pMesh->GetTextureUVIndex(polygon, polygonVertex, FbxLayerElement::eTextureDiffuse);
						break;
				}
				fbxUV = fbxLayerUV->GetDirectArray().GetAt(UVIndex);
				vertexData[fbxCornerIndex].texC = Vector2f(fbxUV.mData[0],fbxUV.mData[1]);
			}
		}
	}

	for (int i = 0; i < vertexData.size(); i++){
		if (vertexData[i].texC == Vector2f(0,0))
			std::cout << "Zero normal detected" << std::endl;
	}

	std::cout << "Finished Loading" << std::endl;

	return true;
}

bool ModelLoader::PopulateIndexData(FbxMesh *pMesh){
	std::cout << "Loading " << pMesh->GetPolygonVertexCount() << " indices" << std::endl;
	FbxVector4 normal;
	for (int i = 0; i < pMesh->GetPolygonCount();i++){
		for (int j = 0; j < pMesh->GetPolygonSize(i); j++){
			indexData.push_back(pMesh->GetPolygonVertex(i,j));
			/*pMesh->GetPolygonVertexNormal(i,j,normal);
			if (!normal.IsZero()){
				std::cout << "Normal x: " << normal.mData[0] << " y:" << normal.mData[1] << " z:" << normal.mData[2] << std::endl;
			}*/
		}
	}

	std::cout << "Finished Loading" << std::endl;

	return true;
}

VertexNT* ModelLoader::GetVertexData(){
	return &vertexData[0];
}

DWORD* ModelLoader::GetIndexData(){
	return &indexData[0];
}

int ModelLoader::GetVertexCount(){
	return vertexData.size();
}

int ModelLoader::GetIndexCount(){
	return indexData.size();
}