#ifndef GAMEENTITY_H
#define GAMEENTITY_H

#include "GameObject.h"
#include "Isohedron.h"
#include "Grid.h"
#include "ExplodeShader.h"
#include <iostream>

#define GRAVITY 1.0f	//rate at which ball decelarates
#define PRIME_LIMIT 2.0f	//maximum amount to which a ball can be primed for fire
#define PRIME_RATE  0.02f	//prime amount that gets added to the ball at each loop while priming
#define SPEED	10.0f
/*
Class to represent a playable game object in this application
*/
class GameEntity{
public:
	GameEntity(void);
	GameEntity(GameObject &object); //create this game entity and set it's game object reference
	//quick access variables which are bound to game object parameters
	Vector3f	*pos;		
	Vector3f	*theta;

	bool		isMoving;

	void MoveFacing(float speed);
	void MoveStrafe(float speed);

	
	void SetObject(GameObject &object); //set this game entity's object reference

	void SetBall(Isohedron	&ball);	//set the ball which this game entity will use
	void SetTerrainRef(Grid	&grid);	//set the terrain reference that this entity will use

	GameObject	*GetGameObject();

	void Update(float dt); //update loop for this entity - gets passed in the delta time to use for updates

	void PrimeBall(); //prime ball - every time this get's called it increases the force at which the ball is thrown - up to a limit
	bool IsBallPrimed();
	void FireBall();	
	void RetrieveBall();

	float GetBallForce();

	~GameEntity(void);

private:
	void ResetBall();

private:
	GameObject		*object;
	Isohedron		*ball;
	Grid			*terrain;

	//variables for the ball operation
	Vector3f		target;
	float			timeFired;
	float			ballForce;
	bool			isBallPrimed;
	bool			isBallFired;
	bool			isBallRetrieved;
};

#endif