#include "GameCamera.h"


GameCamera::GameCamera(void)
{
	position = D3DXVECTOR3(0,0,0);
	rotation = D3DXVECTOR3(0,0,0);
	right = D3DXVECTOR3(1.0f,0.0f,0.0f);
	forward = D3DXVECTOR3(0.0f,0.0f,1.0f);
	defaultRight = D3DXVECTOR3(1.0f,0.0f,0.0f);
	defaultForward = D3DXVECTOR3(0.0f,0.0f,1.0f);

	moveLeftRight = moveBackForward = yaw = pitch = yaw = 0.0f;
	mouseLastPos.x = mouseLastPos.y = 0;
}

GameCamera::~GameCamera(void)
{
}

///SETTERS
void GameCamera::SetPosition(float x, float y, float z){
	position = D3DXVECTOR3(x,y,z);
}

void GameCamera::SetRotation(float x, float y, float z){
	rotation = D3DXVECTOR3(x,y,z);
}

void GameCamera::SetPosition(D3DXVECTOR3 position){
	this->position = position;
}

void GameCamera::SetRotation(D3DXVECTOR3 rotation){
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

void GameCamera::MouseMove(int wndWidth, int wndHeight){	
	POINT mousePos; 
	int mid_x = wndWidth >> 1; //dividing by 2 in a fancy way
	int mid_y = wndHeight >> 1; //dividing by 2 in a fancy way
	
	GetCursorPos(&mousePos);	

	SetCursorPos(mid_x, mid_y);

	yaw -= (float)( (mid_x - mousePos.x) ) / 1000; 
	pitch -= (float)( (mid_y - mousePos.y) ) / 1000;
}

//The Render function uses the position and rotation of the camera to build and update the view matrix.
void GameCamera::Render()
{
	D3DXVECTOR3 lookAt;
	
	D3DXMATRIX rotationMatrix;

	// Setup the up vector.
	up = D3DXVECTOR3(0,1,0);

	// Setup where the camera is looking by default.
	lookAt = D3DXVECTOR3(0,0,1);

	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	//pitch = rotation.x * 0.0174532925f;
	//yaw   = rotation.y * 0.0174532925f;
	//roll  = rotation.z * 0.0174532925f;

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

	position += moveLeftRight*right;
	position += moveBackForward*forward;

	moveLeftRight = 0.0f;
	moveBackForward = 0.0f;

	// Translate the rotated camera position to the location of the viewer.
	lookAt = position + lookAt;

	// Finally create the view matrix from the three updated vectors.
	D3DXMatrixLookAtLH(&mViewMatrix, &position, &lookAt, &up);
}

void GameCamera::GetViewMatrix(D3DXMATRIX& viewMatrix)
{
	viewMatrix = mViewMatrix;
}