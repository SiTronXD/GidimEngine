#include "Input.h"

bool Input::keys[] = { false };
int Input::cursorX = 0;
int Input::cursorY = 0;
int Input::cursorDeltaX = 0;
int Input::cursorDeltaY = 0;


Input::Input()
{
	for (int i = 0; i < MAX_NUM_KEYS; ++i)
	{
		keys[i] = false;
	}
}

Input::~Input() { }

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

bool Input::isKeyDown(KEYS keyCode)
{
	return keys[(unsigned int) keyCode];
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
