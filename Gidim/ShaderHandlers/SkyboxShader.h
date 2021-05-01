#pragma once

#include "../Engine/Graphics/Shader.h"

class SkyboxShader : public Shader
{
private:

public:
	SkyboxShader(Renderer& renderer);
	~SkyboxShader();
};