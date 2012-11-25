#include "GameCamera.h"


GameCamera::GameCamera(bool willBeAttached)
{
	position = rotation = D3DXVECTOR3(0.0f,0.0f,0.0f);
	right = D3DXVECTOR3(1.0f,0.0f,0.0f);
	forward = D3DXVECTOR3(0.0f,0.0f,1.0f);
	defaultRight = D3DXVECTOR3(1.0f,0.0f,0.0f);
	defaultForward = D3DXVECTOR3(0.0f,0.0f,1.0f);

	moveLeftRight = moveBackForward = yaw = pitch = yaw = 0.0f;
	mouseLastPos.x = mouseLastPos.y = 0;
	camMoveFactor = 15.0f;
	attached = willBeAttached;
}

GameCamera::~GameCamera(void)
{
}

void GameCamera::SetPivotPoint(D3DXVECTOR3 &pos){
	pivotPoint = pos;
}

void GameCamera::MoveYawPitch(float yaw, float pitch){	
	this->yaw -= yaw;
	this->pitch -= pitch;
}

//The Render function uses the position and rotation of the camera to build and update the view matrix.
void GameCamera::Render()
{
	D3DXMATRIX rotationMatrix;	 

	// Setup the up vector.
	up = D3DXVECTOR3(0,1,0);

	// Setup where the camera is looking by default.
	lookAt = D3DXVECTOR3(0,0,1);

	if (attached)
		yaw   = rotation.y;

	// Create the rotation matrix from the yaw, pitch, and roll values.
	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, yaw, pitch, 0);

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	D3DXVec3TransformCoord(&lookAt, &defaultForward, &rotationMatrix);
	D3DXVec3Normalize(&lookAt,&lookAt);	

	D3DXMATRIX yawMatrix;
	D3DXMatrixRotationY(&yawMatrix, yaw);
	D3DXVec3TransformNormal(&right, &defaultRight, &yawMatrix);
	D3DXVec3TransformNormal(&up, &up, &rotationMatrix);
	D3DXVec3TransformNormal(&forward, &defaultForward, &rotationMatrix);

	D3DXVECTOR3 camPos;

	if (attached){
		D3DXVECTOR3 pivot;
		D3DXVec3TransformCoord(&pivot,&pivotPoint,&rotationMatrix);
		camPos = position + pivot;
	}
	else{		
		position += moveLeftRight*right;
		position += moveBackForward*forward;
		moveLeftRight = 0.0f;
		moveBackForward = 0.0f;
		camPos = position;
	}

	// Translate the rotated camera position to the location of the viewer.
	lookAt = camPos + lookAt;

	// Finally create the view matrix from the three updated vectors.
	D3DXMatrixLookAtLH(&mViewMatrix, &camPos, &lookAt, &up);
}

void GameCamera::ModifyCamMovement(float amount){
	if (!attached){
		camMoveFactor += amount;
		if (camMoveFactor < 0.0f){
			camMoveFactor = 0.0f;
		}
	}
}

///SETTERS
void GameCamera::SetPosition(float x, float y, float z){
	position = D3DXVECTOR3(x,y,z);
}

void GameCamera::SetRotation(float x, float y, float z){
	rotation = D3DXVECTOR3(x,y,z);
}

void GameCamera::SetPosition(D3DXVECTOR3 &position){
	this->position = position;
}

void GameCamera::SetRotation(D3DXVECTOR3 &rotation){
	this->rotation = rotation;
}

void GameCamera::SetUp(D3DXVECTOR3 up){
	this->up = up;
}

///GETTERS
D3DXVECTOR3 GameCamera::GetPosition(){
	return position;
}

D3DXVECTOR3 GameCamera::GetRotation(){
	return rotation;
}

D3DXVECTOR3 GameCamera::GetLookAtTarget(){
	return lookAt;
}

D3DXVECTOR3 GameCamera::GetPivotPoint(){
	return pivotPoint;
}

void GameCamera::GetViewMatrix(D3DXMATRIX& viewMatrix)
{
	viewMatrix = mViewMatrix;
}