#ifndef MODELLOADER_H
#define MODELLOADER_H

///A CLASS THAT HANDLES LOADING MODEL INFORMATION FOR .FBX MODEL FORMATS
///REQUIRES THE FBXSDK PLUGIN FROM AUTODESK

#include <fbxsdk.h>
#include <vector>
#include <iostream>
#include "Vertex.h"
#include "d3dUtil.h"

class ModelLoader
{
public:
	ModelLoader(void);
	~ModelLoader(void);

	bool	LoadModel(const char* filename);
	
	VertexNT* GetVertexData();
	DWORD*   GetIndexData();

	int		 GetVertexCount();
	int		 GetIndexCount();

private:
	bool GetNodeData(FbxNode *pNode);
	bool GetMeshData(FbxMesh *pMesh);

	int numNodes;

	bool PopulateVertexData(FbxMesh *pMesh);
	bool PopulateIndexData(FbxMesh *pMesh);
private:
	std::vector<VertexNT> vertexData;
	std::vector<DWORD>	  indexData;
};

#endif