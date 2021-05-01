#pragma once

#include "../Engine/Graphics/Shader.h"

class ColorShader : public Shader
{
private:
public:
	ColorShader(Renderer& renderer);
	~ColorShader();
};