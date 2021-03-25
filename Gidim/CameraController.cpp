#include "CameraController.h"
#include "Input.h"
#include "Time.h"
#include "Log.h"

CameraController::CameraController(int windowWidth, int windowHeight)
    : camera(
        3.14f * 0.5f, (float)windowWidth / (float)windowHeight, 0.1f, 1000.0f,
        XMVectorSet(0.0f, 1.0f, -3.0f, 1.0f)
    ),
    mouseSensitivity(0.2f), movementSpeed(3.0f)
{

}

CameraController::~CameraController()
{
}

void CameraController::update()
{
    // Rotation
    float rotX = Input::getCursorDeltaX() * this->mouseSensitivity * 0.01f;
    float rotY = -Input::getCursorDeltaY() * this->mouseSensitivity * 0.01f;

    camera.rotate(XMVectorSet(rotY, rotX, 0.0f, 0.0f));


    // Movement
    int forwardDir = Input::isKeyDown(Keys::W) - Input::isKeyDown(Keys::S);
    int leftDir = Input::isKeyDown(Keys::A) - Input::isKeyDown(Keys::D);
    int upDir = Input::isKeyDown(Keys::E) - Input::isKeyDown(Keys::Q);

    // Apply movement
    XMVECTOR deltaMovement =
        XMVector3Normalize(XMVectorSet(leftDir, upDir, forwardDir, 0.0f)) *
        this->movementSpeed * Time::getDeltaTime();
    camera.move(deltaMovement);
}

Camera& CameraController::getCamera()
{
    return this->camera;
}
