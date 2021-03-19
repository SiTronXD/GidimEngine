#include "Input.h"

bool Input::keys[] = { false };


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

bool Input::isKeyDown(KEYS keyCode)
{
	return keys[(unsigned int) keyCode];
}