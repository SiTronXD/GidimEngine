#pragma once

#include "../../pch.h"

class Camera
{
private:
	XMFLOAT4X4 projectionMatrix;
	XMFLOAT4X4 viewMatrix;

	XMFLOAT3 position;
	XMFLOAT3 lookAt;
	XMFLOAT3 worldUp;

	XMFLOAT3 forward;
	XMFLOAT3 left;
	XMFLOAT3 up;

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

	const XMMATRIX getProjectionMatrix() const;
	const XMMATRIX getViewMatrix();
	
	const XMFLOAT3& getPosition() const;
};