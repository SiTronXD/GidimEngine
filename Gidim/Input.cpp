#include "Input.h"

Input::Input()
{
	for (int i = 0; i < MAX_NUM_KEYS; ++i)
	{
		this->keys[i] = false;
	}
}

Input::~Input() { }


void Input::setKeyDown(unsigned int keyCode)
{
	this->keys[keyCode] = true;
}

void Input::setKeyUp(unsigned int keyCode)
{
	this->keys[keyCode] = false;
}

bool Input::isKeyDown(KEYS keyCode)
{
	return this->keys[(unsigned int) keyCode];
}