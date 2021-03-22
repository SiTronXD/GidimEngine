#include "Camera.h"

void Camera::updateViewMatrix()
{
	this->viewMatrix = XMMatrixLookAtLH(this->position, this->lookAt, this->up);
}

Camera::Camera(float fov, float aspectRatio, float nearZ, float farZ)
{
	this->projectionMatrix = XMMatrixPerspectiveFovLH(fov, aspectRatio, nearZ, farZ);

	this->position = XMVectorSet(0.0f, 0.0f, -3.0f, 1.0f);
	this->lookAt = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	this->up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	this->updateViewMatrix();
}

Camera::~Camera()
{
}

void Camera::setPosition(XMVECTOR newPos)
{
	this->position = newPos;
}

void Camera::move(XMVECTOR delta)
{
	this->position = XMVectorAdd(position, delta);
}

const XMMATRIX& Camera::getProjectionMatrix() const
{
	return this->projectionMatrix;
}

const XMMATRIX& Camera::getViewMatrix()
{
	this->updateViewMatrix();

	return this->viewMatrix;
}

/*const XMMATRIX Camera::getViewProjectionMatrix() const
{
	return XMMATRIX();
}*/
