#include "Input.h"

bool Input::keys[] = { false };
bool Input::lastPressedKeys[] = { false };
int Input::cursorX = 0;
int Input::cursorY = 0;
int Input::cursorDeltaX = 0;
int Input::cursorDeltaY = 0;


Input::Input()
{
	for (unsigned int i = 0; i < MAX_NUM_KEYS; ++i)
	{
		keys[i] = false;
		lastPressedKeys[i] = false;
	}
}

Input::~Input() { }

void Input::updateLastPressedKeys()
{
	for (unsigned int i = 0; i < MAX_NUM_KEYS; ++i)
	{
		lastPressedKeys[i] = keys[i];
	}
}

void Input::setKeyDown(unsigned int keyCode)
{
	keys[keyCode] = true;
}

void Input::setKeyUp(unsigned int keyCode)
{
	keys[keyCode] = false;
}

void Input::setCursorPos(int newX, int newY)
{
	this->cursorX = newX;
	this->cursorY = newY;
}

void Input::setCursorDelta(int newDeltaX, int newDeltaY)
{
	this->cursorDeltaX = newDeltaX;
	this->cursorDeltaY = newDeltaY;
}

bool Input::isKeyDown(Keys keyCode)
{
	return keys[(unsigned int) keyCode];
}

bool Input::isKeyJustPressed(Keys keyCode)
{
	return keys[(unsigned int)keyCode] && !lastPressedKeys[(unsigned int)keyCode];
}

int Input::getCursorX()
{
	return cursorX;
}

int Input::getCursorY()
{
	return cursorY;
}

int Input::getCursorDeltaX()
{
	return cursorDeltaX;
}

int Input::getCursorDeltaY()
{
	return cursorDeltaY;
}
