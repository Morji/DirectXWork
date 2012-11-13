#ifndef _GRID_H
#define _GRID_H

#include "GameObject.h"
#include "d3dUtil.h"
#include "TerrainLoader.h"

#define CELLSPACING		1.0f
#define	HEIGHT_FACTOR	0.2f;

const int TEXTURE_REPEAT = 1;	//how often the texture will repeat over the terrain grid

class Grid : public GameObject
{
public:
	Grid(void);
	~Grid(void);

	bool GenerateGrid(int width, int depth);
	bool GenerateGridFromTGA(char* filename);

private:
	bool InitializeBuffers(DWORD* indices,  VertexNT* vertices);

	float GetHeight(float,float)const;
	void  ComputeNormals();				// computes the normals of the terrain on a per-vertex level
	void  ComputeTextureCoords();		// computes the texture coordinates of the terrain

private:	
	DWORD			*indices;
	VertexNT		*vertices;
	int				gridWidth;
	int				gridDepth;
};

#endif