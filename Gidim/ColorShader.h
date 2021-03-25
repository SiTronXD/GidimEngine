#pragma once

#include "Shader.h"

class ColorShader : public Shader
{
private:
public:
	ColorShader(Renderer& renderer);
	~ColorShader();
};