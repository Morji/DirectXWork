#ifndef MODELOBJECT_H
#define MODELOBJECT_H

#include "GameObject.h"
#include "ModelLoader.h"

class ModelObject :	public GameObject
{
public:
	ModelObject(void);
	~ModelObject(void);

	

	bool LoadModelFromFBX(const char* filename);

private:
	ModelLoader *modelLoader;

};


#endif