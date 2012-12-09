#include "GameEntity.h"

GameEntity::GameEntity(){
	object = 0;
}

GameEntity::GameEntity(GameObject &object){
	this->object = &object;
}

void GameEntity::SetObject(GameObject &object){
	this->object = &object;
}

GameObject *GameEntity::GetGameObject(){
	return this->object;
}

GameEntity::~GameEntity(){
	object = nullptr;
}