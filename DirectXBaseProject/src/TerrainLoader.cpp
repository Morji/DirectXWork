#include "TerrainLoader.h"

TerrainLoader::TerrainLoader(){
	height = 0;
	terrainWidth = terrainDepth = 0;
}

TerrainLoader::~TerrainLoader(){
	delete [] height;
	height = nullptr;
	
}

bool TerrainLoader::LoadTerrain(char* filename){
	height = 0;

	//store the heightmap to a file
	FILE * fTGA = LoadTGA(filename);

	//compute the heights from that file
	if (fTGA != NULL){
		if (!ComputeHeights(fTGA)){
			return false;
		}
	}
	return true;
}

FILE * TerrainLoader::LoadTGA(char* filename){
	FILE * fTGA;										// File pointer to texture file
	fTGA = fopen(filename, "rb");						// Open file for reading

	if(fTGA == NULL){									// If it didn't open....	
		MessageBox(NULL, L"Could not open texture file", L"ERROR", MB_OK);	// Display an error message
		return NULL;														// Exit function
	}

	//we need to see what type of TGA we are loading. 
	if(fread(&Tgaheader, sizeof(TGAHeader), 1, fTGA) == 0){					// Attempt to read 12 byte header from file	
		MessageBox(NULL, L"Could not read file header", L"ERROR", MB_OK);		// If it fails, display an error message 
		if(fTGA != NULL){													// Check to seeiffile is still open		
			fclose(fTGA);													// If it is, close it
		}
		return NULL;														// Exit function
	}

	if(memcmp(UTGAcompare, &Tgaheader, sizeof(Tgaheader)) == 0){			// See if header matches the predefined header
		
		return fTGA;														// If so - return the file
	}
	else{																	// If header matches neither type	
		MessageBox(NULL, L"TGA file be type 2 or type 10 ", L"Invalid Image", MB_OK);	// Display an error
		fclose(fTGA);
		return NULL;																// Exit function
	}

	return NULL;							
}

bool TerrainLoader::ComputeHeights(FILE * fTGA){
	TGA tga;

	if(fread(tga.header, sizeof(tga.header), 1, fTGA) == 0){				// Read TGA header											
		MessageBox(NULL, L"Could not read info header", L"ERROR", MB_OK);	// Display error if not read
		if(fTGA != NULL){													// if file is still open		
			fclose(fTGA);													// Close it
		}
		return false;														// Return failure
	}	

	unsigned int terrainTexture_width	= tga.header[1] * 256 + tga.header[0];			// Determine The TGA Width	(highbyte*256+lowbyte)
	unsigned int terrainTexture_height	= tga.header[3] * 256 + tga.header[2];			// Determine The TGA Height	(highbyte*256+lowbyte)
	unsigned int terrainTexture_bpp		= tga.header[4];								// Determine the bits per pixel

	tga.width		= terrainTexture_width;										// Copy width into local structure						
	tga.height		= terrainTexture_height;									// Copy height into local structure
	tga.bpp			= terrainTexture_bpp;										// Copy BPP into local structure	
	
	if((terrainTexture_width <= 0) || (terrainTexture_height <= 0) || ((terrainTexture_bpp != 24) && (terrainTexture_bpp !=32)))	// Make sure all information is valid
	{
		MessageBox(NULL, L"Invalid texture information", L"ERROR", MB_OK);	// Display Error
		if(fTGA != NULL){													// Check if file is still open		
			fclose(fTGA);													// If so, close it
		}
		return false;														// Return failed
	}

	tga.bytesPerPixel	= (tga.bpp / 8);												// Compute the number of BYTES per pixel
	tga.imageSize		= (tga.bytesPerPixel * tga.width * tga.height);					// Compute the total amout of memory needed to store data
	unsigned char *terrainTexture_imageData	= new unsigned char[tga.imageSize];			// Allocate that much memory

	if(terrainTexture_imageData == NULL){											// If no space was allocated	
		MessageBox(NULL, L"Could not allocate memory for image", L"ERROR", MB_OK);	// Display Error
		fclose(fTGA);														// Close the file
		return false;														// Return failed
	}

	if(fread(terrainTexture_imageData, 1, tga.imageSize, fTGA) != tga.imageSize){	// Attempt to read image data	
		MessageBox(NULL, L"Could not read image data", L"ERROR", MB_OK);			// Display Error
		if(terrainTexture_imageData != NULL){										// If imagedata has data in it		
			free(terrainTexture_imageData);											// Delete data from memory
		}
		fclose(fTGA);														// Close file
		return false;														// Return failed
	}

	height = new float[tga.width*tga.height];

	// Byte Swapping Optimized By Steve Thomas
	for(int cswap = 0; cswap < (int)tga.imageSize; cswap += tga.bytesPerPixel){
		terrainTexture_imageData[cswap] ^= terrainTexture_imageData[cswap+2] ^=
		terrainTexture_imageData[cswap] ^= terrainTexture_imageData[cswap+2];
	}

	fclose(fTGA); // Close file*/

	terrainWidth = tga.width;
	terrainDepth = tga.height;

	///The actual height computing and storing
	for (int i = 0; i < terrainWidth; i++){
		for (int j = 0; j < terrainDepth; j++){
			float pointHeight = terrainTexture_imageData[(j*terrainWidth+i)*3];
			height[j*terrainWidth + i] = pointHeight;
		}
	}

	delete [] terrainTexture_imageData;
	terrainTexture_imageData = nullptr;

	//smooth out the terrain
	for (int i = 0; i < 4; i++)
		SmoothHeights(0.75f);

	return true;															
}

//Smooth out each vertex depending on the nearest vertices to it using the float as the smoothing factor on how much to decrease/increase height
void TerrainLoader::SmoothHeights(float factor){	
	
	float k = factor;
	for (int i = 1; i < terrainWidth; i++)
		for (int j = 0; j < terrainDepth; j++)
			height[j*terrainWidth + i] = height[j*terrainWidth + i - 1]*(1-k) + height[j*terrainWidth + i] * k;

	for (int i = terrainWidth-2; i < -1; i--)
		for (int j = 0; j < terrainDepth; j++)
			height[j*terrainWidth + i] = height[j*terrainWidth + i + 1]*(1-k) + height[j*terrainWidth + i] * k;

	for (int i = 0; i < terrainWidth; i++)
		for (int j = 1; j < terrainDepth; j++)
			height[j*terrainWidth + i] = height[(j-1)*terrainWidth + i]*(1-k) + height[j*terrainWidth + i] * k;

	for (int i = 0; i < terrainWidth; i++)
		for (int j = terrainDepth; j < -1; j--)
			height[j*terrainWidth + i] = height[(j+1)*terrainWidth + i]*(1-k) + height[j*terrainWidth + i] * k;
}

float TerrainLoader::GetHeight(int x, int z){
	return height[z*terrainWidth + x];
}

int TerrainLoader::GetWidth(){
	return terrainWidth;
}

int TerrainLoader::GetDepth(){
	return terrainDepth;
}