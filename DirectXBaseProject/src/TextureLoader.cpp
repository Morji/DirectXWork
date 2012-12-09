#include "TextureLoader.h"


TextureLoader::TextureLoader(void){
	texture = NULL;
	
}

TextureLoader::~TextureLoader(void){
}

bool TextureLoader::Initialize(ID3D10Device* device, WCHAR* filename){
	HRESULT result;

	// Load the texture in.
	result = D3DX10CreateShaderResourceViewFromFile(device, filename, NULL, NULL, &texture, NULL);
	if(FAILED(result)){
		return false;
	}

	return true;
}

bool TextureLoader::Shutdown(){
	// Release the texture resource.	
	ReleaseCOM(texture);
	return true;
}



ID3D10ShaderResourceView* TextureLoader::GetTexture(){
	return texture;
}