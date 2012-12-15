#ifndef ISOHEDRON_H
#define ISOHEDRON_H

#include "GameObject.h"

#define		EXPLOSION_LIMIT		30.0f	//after the explosion has reached this limit - stop it

class Isohedron : public GameObject
{
public:
	Isohedron(void);
	~Isohedron(void);

	void	Render(D3DXMATRIX worldMatrix);

	void	StartExplosion(float rate);
	void	StopExplosion();

	void	Reset();

	float	GetExplosiveForce();

	bool	IsExploding();

private:
	bool SetupArraysAndInitBuffers();
	bool InitializeBuffers(DWORD* indices,  VertexNC* vertices);

	void	Explode();

	float		explosiveRate;

	bool		isExploding;
	bool		isResetting;
	float		explosiveForce;
};


#endif