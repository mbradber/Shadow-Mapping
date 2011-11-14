#pragma once
#include"Utility.h"
#include<dinput.h>

class Camera
{
public:
	Camera();
	~Camera(void){}

	void init(D3DXVECTOR3 pos, D3DXVECTOR3 target, D3DXVECTOR3 up);
	void Update(BYTE* keyboard, DIMOUSESTATE mouse, float delta);
	D3DXMATRIX GetCameraView() const {return cameraViewMatrix;}
	D3DXVECTOR3 GetCameraPosition() const {return cameraPosition;}

private:
	//camera propterties
	D3DXMATRIX cameraViewMatrix;
	D3DXVECTOR3 cameraPosition;
	D3DXVECTOR3 cameraTarget;
	D3DXVECTOR3 cameraDirection;
	D3DXVECTOR3 cameraUp;
	D3DXVECTOR3 cameraSide;
	//buffer movement speed
	float movementBuffer;
	
	//input states
	float deltaTime;
	BYTE keystate[256];
	DIMOUSESTATE mouseState;

	//max pitch variables
	float maxPitch;
	float currentPitch;
};


