#ifndef _GRID_H
#define _GRID_H

#include "GameObject.h"
#include "d3dUtil.h"

#define CELLSPACING		1.0f

class Grid : public GameObject
{
public:
	Grid(void);
	~Grid(void);

	bool GenerateGrid(ID3D10Device* device, int width, int depth);

private:
	bool InitializeBuffers(ID3D10Device* device, DWORD* indices,  VertexC* vertices);

	float GetHeight(float,float)const;

private:	
	DWORD *indices;
	VertexC *vertices;
};

#endif