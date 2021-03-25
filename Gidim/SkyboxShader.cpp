#include "SkyboxShader.h"

SkyboxShader::SkyboxShader(Renderer& renderer)
{
	this->loadFromFile(
		renderer.getDevice(),
		"SkyboxShader_Vert.cso",
		"SkyboxShader_Pix.cso"
	);
}

SkyboxShader::~SkyboxShader()
{
}
