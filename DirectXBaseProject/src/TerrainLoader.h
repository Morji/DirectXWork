/////////////////////////////////////////////////////////////////////////
// TERRAIN LOADER - LOADS IN A HEIGHTMAP AND STORES AN ARRAY OF HEIGHTS
/////////////////////////////////////////////////////////////////////////

#ifndef _TERRAINLOADER_H
#define _TERRAINLOADER_H

#include "d3dUtil.h"
#include "TgaHeader.h"

const unsigned char UTGAcompare[12] = {0,0,2, 0,0,0,0,0,0,0,0,0};	// Uncompressed TGA Header

class TerrainLoader{
public:
	TerrainLoader();
	~TerrainLoader();

	bool	LoadTerrain(char* filename);

	int		GetWidth();
	int		GetDepth();
	float	GetHeight(int x, int z);		// return the height value associated with the X Z coordinate

private:
	FILE	*LoadTGA(char* filename);		// loads in terrain heightmap to a file returns a pointer to the file
	bool	ComputeHeights(FILE * fTGA);	// computes the heights from the TGA file
	void	SmoothHeights(float factor);	// smooths out the heights in the terrain by the given factor
	

private:
	float	*height;	// an array to hold the height values from the heightmap

	int		terrainWidth;
	int		terrainDepth;

	TGAHeader Tgaheader;
};

#endif