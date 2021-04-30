#include "ColorShader.h"

ColorShader::ColorShader(Renderer& renderer)
	: Shader(renderer, "DefaultColorShader_Vert.cso", "DefaultColorShader_Pix.cso") {}

ColorShader::~ColorShader() {}
