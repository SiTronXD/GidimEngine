#pragma once

#include "Camera.h"

class CameraController
{
private:
	Camera camera;

	float mouseSensitivity;
	float movementSpeed;

public:
	CameraController(int windowWidth, int windowHeight);
	~CameraController();

	void update();

	Camera& getCamera();
};