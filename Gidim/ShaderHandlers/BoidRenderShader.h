#pragma once

#include "../Engine/Graphics/Shader.h"

class BoidRenderShader : public Shader
{
private:
public:
	BoidRenderShader(Renderer& renderer);
	~BoidRenderShader();
};