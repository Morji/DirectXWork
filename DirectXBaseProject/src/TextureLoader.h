#ifndef _H_TEXTURELOADER
#define _H_TEXTURELOADER

#include "d3dUtil.h"

///HANDLES LOADING OF TEXTURES
class TextureLoader
{
public:
	TextureLoader(void);
	~TextureLoader(void);

	/*The first two functions will load a texture from a given file name and unload that texture 
	when it is no longer needed.*/
	bool Initialize(ID3D10Device* device, WCHAR* filename);
	void Shutdown();

	/*The GetTexture function returns a pointer to the texture resource so that it 
	can be used for rendering by shaders.*/
	ID3D10ShaderResourceView* GetTexture();

private:

	ID3D10ShaderResourceView* texture;
};

#endif