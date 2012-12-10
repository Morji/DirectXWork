#ifndef EXPLODESHADER_H
#define EXPLODESHADER_H

#include "LightShader.h"

class ExplodeShader : public LightShader
{
public:
	ExplodeShader(void);

	~ExplodeShader(void);

	bool Initialize(ID3D10Device* device, HWND hwnd);
private:
	bool InitializeShader(ID3D10Device* device, HWND hwnd, WCHAR* filename);
};

#endif