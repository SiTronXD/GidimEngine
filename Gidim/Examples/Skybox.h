#pragma once

#include "../Engine/Graphics/MeshData.h"
#include "../Engine/Graphics/Mesh.h"
#include "../ShaderHandlers/SkyboxShader.h"

class Skybox
{
private:
	struct SkyboxBuffer
	{
		float sunDir[3];
		float turbidity;
	} sb{};

	MeshData meshData;
	Mesh mesh;

	SkyboxShader shader;

	ShaderBuffer shaderBuffer;

	Renderer& renderer;

	float timer;

public:
	Skybox(Renderer& renderer);
	~Skybox();

	void draw();
};