#include "BoidRenderShader.h"

BoidRenderShader::BoidRenderShader(Renderer& renderer)
	: Shader(
		renderer, 
		"CompiledShaders/Boid_Vert.cso", "CompiledShaders/Boid_Pix.cso",
		std::vector<D3D11_INPUT_ELEMENT_DESC>
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "SV_InstanceID", 0, DXGI_FORMAT_R32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		}
	) {}

BoidRenderShader::~BoidRenderShader() {}
