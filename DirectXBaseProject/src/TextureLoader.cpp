#include "TextureLoader.h"


TextureLoader::TextureLoader(void){
	texture = 0;
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

void TextureLoader::Shutdown(){
	// Release the texture resource.
	if(texture){
		texture->Release();
		texture = 0;
	}
	return;
}

ID3D10ShaderResourceView* TextureLoader::GetTexture(){
	return texture;
}