#include "TextureShader.h"

TextureShader::TextureShader(Renderer& renderer)
	: Shader(renderer, "DefaultShader_Vert.cso", "DefaultShader_Pix.cso") {}

TextureShader::~TextureShader() {}