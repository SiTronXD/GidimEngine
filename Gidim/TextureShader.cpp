#include "TextureShader.h"

TextureShader::TextureShader(Renderer& renderer)
{ 
	this->loadFromFile(
		renderer.getDevice(),
		"DefaultShader_Vert.cso",
		"DefaultShader_Pix.cso"
	);
}

TextureShader::~TextureShader()
{ }
