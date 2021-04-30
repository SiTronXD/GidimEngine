#include "SkyboxShader.h"

SkyboxShader::SkyboxShader(Renderer& renderer)
	: Shader(renderer, "SkyboxShader_Vert.cso", "SkyboxShader_Pix.cso") {}

SkyboxShader::~SkyboxShader() {}
