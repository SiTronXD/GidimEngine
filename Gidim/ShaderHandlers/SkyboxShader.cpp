#include "SkyboxShader.h"

SkyboxShader::SkyboxShader(Renderer& renderer)
	: Shader(renderer, "CompiledShaders/SkyboxShader_Vert.cso", "CompiledShaders/SkyboxShader_Pix.cso") {}

SkyboxShader::~SkyboxShader() {}
