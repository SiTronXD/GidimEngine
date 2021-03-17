#pragma once

#include <d3d11.h>

#include "Window.h"

class AppWindow : public Window
{
private:
public:
	AppWindow();
	~AppWindow();

	// Inherited via Window
	virtual void onCreate() override;
	virtual void onUpdate() override;
	virtual void onDestroy() override;
};