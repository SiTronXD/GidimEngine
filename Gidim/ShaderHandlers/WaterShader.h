#pragma once

#include "../Engine/Graphics/Shader.h"

class WaterShader : public Shader
{
private:
public:
	WaterShader(Renderer& renderer);
	~WaterShader();
};