#include "WaterShader.h"

WaterShader::WaterShader(Renderer& renderer)
	: Shader(renderer, "WaterShader_Vert.cso", "WaterShader_Pix.cso") {}

WaterShader::~WaterShader() {}
