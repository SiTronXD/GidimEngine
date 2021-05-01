#include "TextureShader.h"

TextureShader::TextureShader(Renderer& renderer)
	: Shader(renderer, "CompiledShaders/DefaultShader_Vert.cso", "CompiledShaders/DefaultShader_Pix.cso") {}

TextureShader::~TextureShader() {}