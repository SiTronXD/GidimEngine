#pragma once

#include "../Engine/Graphics/Shader.h"

class TextureShader : public Shader
{
private:
public:
	TextureShader(Renderer& renderer);
	~TextureShader();
};