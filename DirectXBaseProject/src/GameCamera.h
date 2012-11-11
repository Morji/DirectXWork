#ifndef _GAMECAMERA_H_
#define _GAMECAMERA_H_

#include "d3dUtil.h"

#define camMoveFactor 10.0f

class GameCamera
{
public:
	GameCamera(void);
	~GameCamera(void);

	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);

	void SetPosition(D3DXVECTOR3 position);
	void SetRotation(D3DXVECTOR3 rotation);
	void SetUp(D3DXVECTOR3 up);

	D3DXVECTOR3 GetPosition();
	D3DXVECTOR3 GetRotation();

	void Render();
	void MouseMove(int wndWidth, int wndHeight);
	void GetViewMatrix(D3DXMATRIX& viewMatrix);

	float moveLeftRight,moveBackForward;
	float yaw, pitch, roll;
private:
	D3DXVECTOR3 position;
	D3DXVECTOR3 rotation;
	D3DXVECTOR3 target;
	D3DXVECTOR3 up;
	D3DXVECTOR3 right;
	D3DXVECTOR3 defaultForward;
	D3DXVECTOR3 defaultRight;
	D3DXVECTOR3 forward;
	D3DXMATRIX mViewMatrix;

	POINT mouseLastPos;

};


#endif