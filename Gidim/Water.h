#pragma once

#include "Mesh.h"
#include "ComputeShader.h"
#include "WaterShader.h"
#include "ShaderBuffer.h"

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
		float waveDirectionTendency;
	} scb{};

	struct SpectrumInterpolationBuffer
	{
		int gridWidth;
		int gridHeight;

		float horizontalSize;
		float time;
	} sib{};

	struct ButterflyTextureBuffer
	{
		int gridSize;
		int padding[3];
	} btb{};

	struct ButterflyOperationBuffer
	{
		int stage;
		int pingPong;
		int direction;
		int padding;
	} bob{};

	struct InvPermBuffer
	{
		int gridSize;
		int pingPong;
		int padding[2];
	} ipb{};

	Renderer& renderer;

	MeshData meshData;
	Mesh mesh;

	WaterShader shader;

	ComputeShader spectrumCreatorShader;
	ComputeShader spectrumInterpolatorShader;
	ComputeShader butterflyTextureShader;
	ComputeShader butterflyOperationsShader;
	ComputeShader invPermShader;

	Texture initialSpectrumTexture;
	Texture butterflyTexture;
	Texture finalSpectrumTextureX;
	Texture finalSpectrumTextureY;
	Texture finalSpectrumTextureZ;
	Texture displacementTexture;

	ShaderBuffer spectrumCreatorShaderBuffer;
	ShaderBuffer spectrumInterpolationShaderBuffer;
	ShaderBuffer butterflyTextureShaderBuffer;
	ShaderBuffer butterflyOperationShaderBuffer;
	ShaderBuffer invPermShaderBuffer;

	int numMultiplicationStages;

	float timer;

	bool displaceHorizontally;

	void runIFFT(Texture& currentSpectrumTexture);

public:
	Water(Renderer& renderer);
	~Water();

	void toggleHorizontalDisplacement();

	void draw();
};