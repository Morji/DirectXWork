#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

//#ifdef _MSVC
#pragma warning(disable:4244)	// double -> float conversion
#define _CRT_SECURE_NO_WARNINGS
//#endif //_MSVC
#include "d3dUtil.h"
#include "GameTimer.h"
#include "Vertex.h"
#include "TextureLoader.h"
#include <string>

#pragma warning(disable:4305) // double -> float

/*
Base Game Object Class: This handles basic setup and drawing
init() function creates a basic 2d triangle
*/
class GameObject
{

public:
	D3DXMATRIX objMatrix;
	D3DXVECTOR3 pos, theta, scale;	

public:
	GameObject(): mVertexCount(0), mIndexCount(0), mNumFaces(0), md3dDevice(0), mVB(0), mIB(0), scale(1,1,1),pos(0,0,0),theta(0,0,0)
	{
		diffuseMap = specularMap = 0;
	}
	virtual ~GameObject()
	{
		Shutdown();
	}

	void setTrans(D3DXMATRIX worldMatrix);

	bool Initialize(ID3D10Device* device);

	bool InitializeWithTexture(ID3D10Device* device, WCHAR* diffuseMapTex, WCHAR* specularMapTex);

	bool InitializeWithMultiTexture(ID3D10Device* device, WCHAR* specularMapTex, WCHAR* blendMapTex, WCHAR* diffuseMapRV1Tex,
																									 WCHAR* diffuseMapRV2Tex,
																									 WCHAR* diffuseMapRV3Tex);
	
	void Shutdown();
	void Render(ID3D10Device* device, D3DXMATRIX worldMatrix);

	ID3D10ShaderResourceView* GetDiffuseTexture();
	ID3D10ShaderResourceView* GetSpecularTexture();
	ID3D10ShaderResourceView* GetBlendTexture();
	ID3D10ShaderResourceView* GetDiffuseMap(int rvWhich);

	int						  GetIndexCount();
	///////////////////////////////////////////////
private:
	
	void ShutdownBuffers();
	void RenderBuffers(ID3D10Device* device);

	TextureLoader* specularMap;
	TextureLoader* diffuseMap;	
	TextureLoader* blendMap;
	TextureLoader* diffuseMapRV[3];
	///////////////////////////////////////////////
protected:
	DWORD mVertexCount;
	DWORD mIndexCount;
	DWORD mNumFaces;

	ID3D10Device* md3dDevice;
	ID3D10Buffer* mVB;
	ID3D10Buffer* mIB;

	bool InitializeBuffers(ID3D10Device* device, DWORD* indices,  Vertex* vertices);
	virtual bool SetupArraysAndInitBuffers(ID3D10Device* device);

	bool LoadTexture(ID3D10Device* device, WCHAR* diffuseMapTex, WCHAR* specularMapTex);
	bool LoadMultiTexture(ID3D10Device* device, WCHAR* specularMapTex, WCHAR* blendMapTex, WCHAR* diffuseMapRV1Tex,
																						   WCHAR* diffuseMapRV2Tex,
																						   WCHAR* diffuseMapRV3Tex);
	void ReleaseTexture();
};

#endif // GAMEOBJECT_H

