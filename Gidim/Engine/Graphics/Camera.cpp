#include "Camera.h"
#include "../SMath.h"

void Camera::updateDirectionVectors()
{
	XMVECTOR tempLookAt = XMLoadFloat3(&this->lookAt);
	XMVECTOR tempPosition = XMLoadFloat3(&this->position);
	XMVECTOR tempWorldUp = XMLoadFloat3(&this->worldUp);

	// Forward
	XMVECTOR tempForward = XMVectorSubtract(tempLookAt, tempPosition);
	tempForward = XMVector3Normalize(tempForward);
	XMStoreFloat3(&this->forward, tempForward);

	// Left
	XMVECTOR tempLeft = XMVector3Cross(tempForward, tempWorldUp);
	tempLeft = XMVector3Normalize(tempLeft);
	XMStoreFloat3(&this->left, tempLeft);

	// Up
	XMVECTOR tempUp = XMVector3Cross(tempLeft, tempForward);
	tempUp = XMVector3Normalize(tempUp);
	XMStoreFloat3(&this->up, tempUp);
}

void Camera::updateViewMatrix()
{
	XMVECTOR tempPosition = XMLoadFloat3(&this->position);
	XMVECTOR tempForward = XMLoadFloat3(&this->forward);
	XMVECTOR tempWorldUp = XMLoadFloat3(&this->worldUp);

	XMMATRIX tempViewMatrix = XMMatrixLookAtLH(
		tempPosition, 
		XMVectorAdd(tempPosition, tempForward), 
		tempWorldUp
	);

	XMStoreFloat4x4(&this->viewMatrix, tempViewMatrix);
}

Camera::Camera(float fov, float aspectRatio, float nearZ, float farZ, XMVECTOR startPos)
	: pitch(0.0f), yaw(0.0f)
{
	// Projection matrix
	XMStoreFloat4x4(
		&this->projectionMatrix, 
		XMMatrixPerspectiveFovLH(fov, aspectRatio, nearZ, farZ)
	);

	// Vectors
	XMStoreFloat3(&this->position, startPos);
	XMStoreFloat3(&this->lookAt, XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f));
	XMStoreFloat3(&this->worldUp, XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

	this->updateDirectionVectors();
	this->updateViewMatrix();
}

Camera::~Camera()
{
}

void Camera::setPosition(XMVECTOR newPos)
{
	XMStoreFloat3(&this->position, newPos);
}

void Camera::move(XMVECTOR dirV)
{
	XMVECTOR tempLeft = XMLoadFloat3(&this->left);
	XMVECTOR tempWorldUp = XMLoadFloat3(&this->worldUp);
	XMVECTOR tempForward = XMLoadFloat3(&this->forward);
	XMVECTOR tempPosition = XMLoadFloat3(&this->position);

	XMVECTOR delta = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	XMFLOAT4 dir;
	XMStoreFloat4(&dir, dirV);

	// Apply direction to delta
	delta = XMVectorAdd(tempLeft * dir.x, delta);
	delta = XMVectorAdd(tempWorldUp * dir.y, delta);
	delta = XMVectorAdd(tempForward * dir.z, delta);

	// Apply movement to position
	XMStoreFloat3(&this->position, XMVectorAdd(tempPosition, delta));
}

void Camera::rotate(XMVECTOR dirV)
{
	XMVECTOR tempPosition = XMLoadFloat3(&this->position);

	XMFLOAT4 dir;
	XMStoreFloat4(&dir, dirV);

	this->yaw += dir.x;
	this->pitch += dir.y;

	// Clamp limit
	this->pitch = SMath::clamp(
		this->pitch, 
		-SMath::PI * 0.5f * 0.99f, 
		 SMath::PI * 0.5f * 0.99f
	);

	// Create rotation matrix from new pitch, yaw and roll
	XMStoreFloat3(
		&this->lookAt, 
		XMVectorAdd(tempPosition, 
			XMVectorSet(
				(float) (sin(yaw) * cos(pitch)),
				(float) sin(pitch),
				(float) (cos(yaw) * cos(pitch)),
				0.0f
			)
		)
	);

	this->updateDirectionVectors();
}

const XMMATRIX Camera::getProjectionMatrix() const
{
	XMMATRIX tempProjectionMatrix = XMLoadFloat4x4(&this->projectionMatrix);

	return tempProjectionMatrix;
}

const XMMATRIX Camera::getViewMatrix()
{
	this->updateViewMatrix();

	XMMATRIX tempViewMatrix = XMLoadFloat4x4(&this->viewMatrix);

	return tempViewMatrix;
}

const XMFLOAT3& Camera::getPosition() const
{
	return this->position;
}