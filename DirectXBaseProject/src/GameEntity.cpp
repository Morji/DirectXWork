#include "GameEntity.h"


GameEntity::GameEntity(){
	object = 0;
	ball = 0;
	pos = 0;
	theta = 0;
}

GameEntity::GameEntity(GameObject &object){
	ball = 0;
	this->object = &object;
	pos = &object.pos;
	theta = &object.theta;
	ResetBall();
}

void GameEntity::Update(float dt){
	if (isBallFired){		
		ball->pos += target;		
		ball->pos.y -= GRAVITY*timeFired;//semi-realistic falling speed
		timeFired+=dt;
		if (ball->pos.y <= terrain->GetHeight(ball->pos.x,ball->pos.z)){
			isBallFired = false;
			ball->StartExplosion(ballForce/8);
		}
	}
	else if (isBallRetrieved){		
		ball->pos.x = pos->x;
		ball->pos.y = pos->y + 20;
		ball->pos.z = pos->z;
		if (!isBallPrimed){
			ball->theta += Vector3f(0,-0.005f,0);
		}
	}
	else{
		if (!ball->IsExploding()){
			RetrieveBall();
		}
	}
}

void GameEntity::PrimeBall(){
	if (isBallRetrieved){
		ballForce+=PRIME_RATE;
		if (ballForce > PRIME_LIMIT)
			ballForce = PRIME_LIMIT;
		ball->theta += Vector3f(0,ballForce/8,0);//spin ball as it is priming
		isBallPrimed = true;
	}
}

void GameEntity::FireBall(){
	if (ball){
		if (!isBallFired){
			std::cout << "BallForce is " << ballForce << std::endl;
			target = (Vector3f(object->objMatrix.m[2][0],0,object->objMatrix.m[2][2]))*ballForce;
			isBallFired = true;
			isBallRetrieved = false;
			isBallPrimed = false;
		}
	}
}

void GameEntity::RetrieveBall(){
	if (!isBallFired){
		ResetBall();
		ball->StopExplosion();		
	}
}

bool GameEntity::IsBallPrimed(){
	return isBallPrimed;
}

void GameEntity::ResetBall(){
	timeFired = 0.0f;
	ballForce = 0.5f;
	isBallFired = false;
	isBallRetrieved = true;
	isBallPrimed = false;
}

void GameEntity::SetObject(GameObject &object){
	this->object = &object;
	pos = &object.pos;
	theta = &object.theta;
}

float GameEntity::GetBallForce(){
	return this->ballForce;
}

void GameEntity::SetTerrainRef(Grid &grid){
	this->terrain = &grid;
}

void GameEntity::SetBall(Isohedron &ball){
	this->ball = &ball;
}

GameObject *GameEntity::GetGameObject(){
	return this->object;
}

GameEntity::~GameEntity(){
	object = nullptr;
	ball = nullptr;	
}