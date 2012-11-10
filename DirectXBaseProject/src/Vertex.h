#ifndef VERTEX_H
#define VERTEX_H

#include "d3dUtil.h"

typedef D3DXVECTOR2 Vector2f;
typedef D3DXVECTOR3 Vector3f;
typedef D3DXVECTOR4 Vector4f;

struct Vertex
{
	Vector3f	pos;
	Vector3f	normal;
	Vector2f	texC;
};
 
#endif // VERTEX_H