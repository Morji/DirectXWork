#ifndef ISOHEDRON_H
#define ISOHEDRON_H

#include "GameObject.h"

class Isohedron : public GameObject
{
public:
	Isohedron(void);
	~Isohedron(void);

private:
	bool SetupArraysAndInitBuffers();
	bool InitializeBuffers(DWORD* indices,  VertexNC* vertices);
};


#endif