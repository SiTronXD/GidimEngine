#pragma once

#include "../../Engine/Graphics/Mesh.h"
#include "../../Engine/Graphics/ComputeShader.h"
#include "../../Engine/Graphics/ShaderBuffer.h"
#include "../../ShaderHandlers/WaterShader.h"
#include "Skybox.h"

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

	static const int NORMAL_MAP_SIZE_SCALE;

	struct SpectrumCreatorBuffer
	{
		int gridWidth;
		int gridHeight;

		XMFLOAT2 windDirection;

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

	struct HeightToNormalBuffer
	{
		int gridWidth;
		int gridHeight;
		int normalMapWidth;
		int normalMapHeight;
		float unitLength;

		int padding[3];
	} htnb{};

	struct FoamMaskBuffer
	{
		int gridWidth;
		int gridHeight;

		float lambdaDispScale;

		int padding;
	} fmb{};

	struct WaterVertexBuffer 
	{
		float lambdaDispScale;

		float padding[3];
	} wvb;

	struct WaterBuffer
	{
		XMFLOAT3 cameraPosition;
		float padding1;
		XMFLOAT3 sunDirection;
		float padding2;
	} wb{};

	Renderer& renderer;

	MeshData meshData;
	Mesh mesh;

	WaterShader meshShader;

	// FFT compute shaders
	ComputeShader spectrumCreatorShader;
	ComputeShader spectrumInterpolatorShader;
	ComputeShader butterflyTextureShader;
	ComputeShader butterflyOperationsShader;
	ComputeShader invPermShader;

	ComputeShader displacementToNormalShader;
	ComputeShader foamMaskShader;

	// FFT texture
	Texture initialSpectrumTexture;
	Texture butterflyTexture;
	Texture finalSpectrumTextureX;
	Texture finalSpectrumTextureY;
	Texture finalSpectrumTextureZ;
	Texture displacementTexture;

	Texture normalMapTexture;
	Texture foamMaskTexture;
	Texture foamTexture;

	// FFT shader buffers
	ShaderBuffer spectrumCreatorShaderBuffer;
	ShaderBuffer spectrumInterpolationShaderBuffer;
	ShaderBuffer butterflyTextureShaderBuffer;
	ShaderBuffer butterflyOperationShaderBuffer;
	ShaderBuffer invPermShaderBuffer;

	ShaderBuffer disToNormShaderBuffer;
	ShaderBuffer foamMaskShaderBuffer;

	ShaderBuffer waterVertexShaderBuffer;
	ShaderBuffer waterShaderBuffer;

	Skybox* skybox;
	CubeMap* reflectedCubeMap;

	int numMultiplicationStages;
	int numPlaneRepetitions;

	float planeLength;
	float lambdaDisplacementScale;
	float timer;

	bool displaceHorizontally;

	void runIFFT(Texture& currentSpectrumTexture);

public:
	Water(Renderer& renderer);
	~Water();

	void setSkybox(Skybox& skybox);
	void toggleHorizontalDisplacement();

	void draw();
};