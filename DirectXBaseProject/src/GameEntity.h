#ifndef GAMEENTITY_H
#define GAMEENTITY_H

#include "GameObject.h"

/*
Class to represent a playable game object in this application
*/
class GameEntity{
public:
	GameEntity(void);

	GameEntity(GameObject &object);

	void SetObject(GameObject &object);

	GameObject	*GetGameObject();

	~GameEntity(void);

private:
	GameObject		*object;
};

#endif