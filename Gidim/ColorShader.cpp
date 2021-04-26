#include "ColorShader.h"

ColorShader::ColorShader(Renderer& renderer)
	: Shader(renderer)
{
	this->loadFromFile(
		renderer.getDevice(),
		"DefaultColorShader_Vert.cso",
		"DefaultColorShader_Pix.cso"
	);
}

ColorShader::~ColorShader()
{
}
