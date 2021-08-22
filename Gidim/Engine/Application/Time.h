#pragma once

#include <iostream>
#include <chrono>
#include <ctime>

class Time
{
private:
	static float deltaTime;
	static float gameTimer;
	static float lastGameTimer;

	static int numFrames;
	static int currentFPS;

	static bool oneSecondPassed;

	std::chrono::system_clock::time_point lastTime;
	std::chrono::system_clock::time_point currentTime;

public:
	Time();
	~Time();

	void updateDeltaTime();

	static const float& getDeltaTime();
	static const int& getFramesPerSecond();
	static const bool hasOneSecondPassed();
	static const float& getTimeSinceStart();
};