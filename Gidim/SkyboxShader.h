#pragma once

#include "Shader.h"

class SkyboxShader : public Shader
{
private:

public:
	SkyboxShader(Renderer& renderer);
	~SkyboxShader();
};