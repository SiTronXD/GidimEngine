#pragma once

#include "Shader.h"

class TextureShader : public Shader
{
private:
public:
	TextureShader(Renderer& renderer);
	~TextureShader();
};