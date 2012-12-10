#ifndef _GRID_H
#define _GRID_H

#include "GameObject.h"
#include "d3dUtil.h"
#include "TerrainLoader.h"

#define CELLSPACING		1.0f
#define	HEIGHT_FACTOR	0.2f;

class Grid : public GameObject
{
public:
	Grid(void);
	~Grid(void);

	bool GenerateGrid(int width, int depth, int texRepeat = 1);
	bool GenerateGridFromTGA(char* filename);

	float GetMaxHeight();

	float GetHeight(float x, float z);

	void  AnimateUV(float dt);

private:
	bool InitializeBuffers(DWORD* indices,  VertexNT* vertices);

	
	void  ComputeNormals()const;				// computes the normals of the terrain on a per-vertex level
	void  ComputeTextureCoords(const int repeatAmount = 1)const;		// computes the texture coordinates of the terrain

private:	
	DWORD			*indices;
	VertexNT		*vertices;
	int				gridWidth;
	int				gridDepth;
	float			maxHeight;

	Vector2f		texOffset;
	float			*heightData;			//array containing the height data for ease of access for terrain collision
};

#endif