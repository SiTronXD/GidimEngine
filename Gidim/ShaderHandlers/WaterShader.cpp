#include "WaterShader.h"

WaterShader::WaterShader(Renderer& renderer)
	: Shader(renderer, "CompiledShaders/WaterShader_Vert.cso", "CompiledShaders/WaterShader_Pix.cso") {}

WaterShader::~WaterShader() {}
