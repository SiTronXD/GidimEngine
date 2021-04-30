#pragma once

#include "Mesh.h"
#include "ComputeShader.h"
#include "WaterShader.h"

class Water
{
private:
	static const int GRID_WIDTH;
	static const int GRID_HEIGHT;
	static const float WIND_DIR_X;
	static const float WIND_DIR_Y;
	static const float HORIZONTAL_SIZE;
	static const float WIND_SPEED;
	static const float AMPLITUDE;

	struct SpectrumCreatorBuffer
	{
		int gridWidth;
		int gridHeight;

		float windDirection[2];

		float horizontalSize;
		float windSpeed;
		float amplitude;
		float padding;
	};

	struct SpectrumInterpolationBuffer
	{
		int gridWidth;
		int gridHeight;

		float horizontalSize;
		float time;
	};

	struct ButterflyTextureBuffer
	{
		int gridSize;
		int padding[3];
	};

	struct ButterflyOperationBuffer
	{
		int stage;
		int pingPong;
		int direction;
		int padding;
	};

	struct InvPermBuffer
	{
		int gridSize;
		int pingPong;
		int padding[2];
	};

	Renderer& renderer;

	MeshData meshData;
	Mesh mesh;

	WaterShader shader;

	ComputeShader spectrumCreatorShader;
	ComputeShader spectrumInterpolatorShader;
	ComputeShader butterflyTextureShader;
	ComputeShader butterflyOperationsShader;
	ComputeShader invPermShader;

	Texture spectrumTexture0;
	Texture spectrumTexture1;
	Texture finalSpectrumTexture;
	Texture butterflyTexture;
	Texture pingPongTexture;
	Texture displacementTexture;

	SDXBuffer spectrumCreatorShaderBuffer;
	SDXBuffer spectrumInterpolationShaderBuffer;
	SDXBuffer butterflyTextureShaderBuffer;
	SDXBuffer butterflyOperationShaderBuffer;
	SDXBuffer invPermShaderBuffer;

	int numMultiplicationStages;

	float timer;

public:
	Water(Renderer& renderer);
	~Water();

	void draw();
};