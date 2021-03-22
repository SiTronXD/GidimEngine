#pragma once

#include <DirectXMath.h>

using namespace DirectX;

class Camera
{
private:
	XMMATRIX projectionMatrix;
	XMMATRIX viewMatrix;

	XMVECTOR position;
	XMVECTOR lookAt;
	XMVECTOR up;

	void updateViewMatrix();

public:
	Camera(float fov, float aspectRatio, float nearZ, float farZ);
	~Camera();

	void setPosition(XMVECTOR newPos);
	void move(XMVECTOR delta);

	const XMMATRIX& getProjectionMatrix() const;
	const XMMATRIX& getViewMatrix();
	//const XMMATRIX getViewProjectionMatrix() const;
};