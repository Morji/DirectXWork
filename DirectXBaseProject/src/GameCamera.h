#ifndef _GAMECAMERA_H_
#define _GAMECAMERA_H_

#include "d3dUtil.h"

class GameCamera
{
public:
	GameCamera(bool willBeAttached = false);

	~GameCamera(void);

	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);

	void SetPosition(D3DXVECTOR3 &position);
	void SetRotation(D3DXVECTOR3 &rotation);
	void SetUp(D3DXVECTOR3 up);

	D3DXVECTOR3 GetPosition();
	D3DXVECTOR3 GetLookAtTarget();
	D3DXVECTOR3 GetRotation();
	D3DXVECTOR3	GetPivotPoint();

	void Render();
	void MoveYawPitch(float yaw, float pitch);
	void GetViewMatrix(D3DXMATRIX& viewMatrix);
	void ModifyCamMovement(float amount);
	void SetPivotPoint(D3DXVECTOR3 &pos);	//for a third person camera - decide the distance from the character

	float moveLeftRight,moveBackForward;
	float yaw, pitch, roll;
	float camMoveFactor;

private:	
	D3DXVECTOR3 position;
	D3DXVECTOR3 rotation;
	bool		attached;

	D3DXVECTOR3	pivotPoint;

	D3DXVECTOR3 lookAt;
	D3DXVECTOR3 up;
	D3DXVECTOR3 right;
	D3DXVECTOR3 defaultForward;
	D3DXVECTOR3 defaultRight;
	D3DXVECTOR3 forward;
	D3DXMATRIX mViewMatrix;

	POINT		mouseLastPos;

};


#endif