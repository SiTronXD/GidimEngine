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
	XMVECTOR worldUp;

	XMVECTOR forward;
	XMVECTOR left;
	XMVECTOR up;

	float pitch;
	float yaw;

	void updateDirectionVectors();
	void updateViewMatrix();

public:
	Camera(float fov, float aspectRatio, float nearZ, float farZ, XMVECTOR startPos);
	~Camera();

	void setPosition(XMVECTOR newPos);
	void move(XMVECTOR dirV);
	void rotate(XMVECTOR dirV);

	const XMMATRIX& getProjectionMatrix() const;
	const XMMATRIX& getViewMatrix();
	//const XMMATRIX getViewProjectionMatrix() const;
};