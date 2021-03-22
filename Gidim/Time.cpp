#include "Time.h"

float Time::deltaTime = 0.0f;
float Time::gameTimer = 0.0f;
float Time::lastGameTimer = 0.0f;
int Time::numFrames = 0;
int Time::currentFPS = 1;
bool Time::oneSecondPassed = false;

Time::Time()
{
	this->lastTime = std::chrono::system_clock::now();
	this->updateDeltaTime();
}

Time::~Time()
{
}

void Time::updateDeltaTime()
{
	numFrames++;

	// Update elapsed time
	this->currentTime = std::chrono::system_clock::now();
	std::chrono::duration<float> elapsedSeconds = currentTime - lastTime;
	lastTime = currentTime;

	// Update delta time
	deltaTime = elapsedSeconds.count();

	// Update to check if one second has passed
	gameTimer += deltaTime;
	oneSecondPassed = (int) gameTimer != (int) lastGameTimer;
	lastGameTimer = gameTimer;

	// Update fps
	if (oneSecondPassed)
	{
		currentFPS = (float) numFrames;

		numFrames = 0;
	}
}

const float& Time::getDeltaTime()
{
	return deltaTime;
}

const bool Time::hasOneSecondPassed()
{
	return oneSecondPassed;
}

const int& Time::getFramesPerSecond()
{
	return currentFPS;
}
