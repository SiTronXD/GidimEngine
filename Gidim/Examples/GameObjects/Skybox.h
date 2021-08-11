#pragma once

#include "../../Engine/Graphics/MeshData.h"
#include "../../Engine/Graphics/Mesh.h"
#include "../../Engine/Graphics/CubeMap.h"
#include "../../Engine/Graphics/ComputeShader.h"
#include "../../ShaderHandlers/SkyboxShader.h"

class Skybox
{
private:
	static const int CUBE_FACE_WIDTH = 256;
	static const int CUBE_FACE_HEIGHT = 256;

	struct SkyboxBuffer
	{
		XMFLOAT3 sunDir;
		float turbidity;

		int faceWidth;
		int faceHeight;

		int padding[2];
	} sb{};

	MeshData meshData;
	Mesh mesh;

	SkyboxShader shader;

	ShaderBuffer shaderBuffer;

	CubeMap skyCubeMap;
	ComputeShader preethamCreatorShader;

	Renderer& renderer;

	XMFLOAT3 sunDir;

	float timer;

public:
	Skybox(Renderer& renderer);
	~Skybox();

	void draw();

	CubeMap& getCubeMap();

	const XMFLOAT3& getSunDir() const;
};