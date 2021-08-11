#include "BoidRenderShader.h"

BoidRenderShader::BoidRenderShader(Renderer& renderer)
	: Shader(renderer, "CompiledShaders/Boid_Vert.cso", "CompiledShaders/Boid_Pix.cso") {}

BoidRenderShader::~BoidRenderShader() {}
