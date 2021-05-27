#include "ComputeShader.h"
#include "../Dev/Helpers.h"

ComputeShader::ComputeShader(Renderer& renderer, int threadGroupX, int threadGroupY)
	: threadGroupX(threadGroupX), threadGroupY(threadGroupY),
	computeShader(nullptr), uavNULL{ NULL }, constantBufferNULL{ NULL },
	device(renderer.getDevice()), deviceContext(renderer.getDeviceContext())
{ }

ComputeShader::~ComputeShader()
{
	S_RELEASE(this->computeShader);
}

bool ComputeShader::createFromFile(std::string path)
{
	// Deallocate old texture, if it exists
	S_RELEASE(this->computeShader);

	// Only accept shader model 5.0 for best performance and features, ignore
	// shader model 4.0 for now
	if (this->device->GetFeatureLevel() < D3D_FEATURE_LEVEL_11_0)
	{
		Log::error(
			"This GPU does not support shader model 5.0, compute shader will not be loaded."
		);

		return false;
	}

	// Open compute shader file
	std::ifstream csFile(path, std::ios::binary);

	// Check if the file could not be found or opened
	if (csFile.fail())
	{
		Log::error("Could not open compute shader file from: " + path);

		return false;
	}
	std::vector<char> csData =
	{
		std::istreambuf_iterator<char>(csFile),
		std::istreambuf_iterator<char>()
	};

	// Create compute shader
	HRESULT result = this->device->CreateComputeShader(
		csData.data(), csData.size(), nullptr, &this->computeShader
	);
	if (FAILED(result))
	{
		Log::error("Failed calling device->CreateComputerShader().");

		return false;
	}

	return true;
}

void ComputeShader::run()
{
	// Set
	this->deviceContext->CSSetShader(this->computeShader, NULL, 0);
	this->deviceContext->CSSetUnorderedAccessViews(
		0, this->renderTextureUAVs.size(), VECTOR_ADDRESS(this->renderTextureUAVs), NULL
	);
	this->deviceContext->CSSetConstantBuffers(
		0, this->constantBuffers.size(), VECTOR_ADDRESS(this->constantBuffers)
	);

	// Run
	this->deviceContext->Dispatch(this->threadGroupX, this->threadGroupY, 1);

	// Reset
	this->deviceContext->CSSetShader(NULL, NULL, 0);
	this->deviceContext->CSSetUnorderedAccessViews(0, NUM_MAX_RENDER_TEXTURES, uavNULL, NULL);
	this->deviceContext->CSSetConstantBuffers(0, NUM_MAX_CONSTANT_BUFFERS, constantBufferNULL);

	// Recreate SRVs after dispatch
	for(size_t i = 0; i < this->renderTextures.size(); ++i)
		this->renderTextures[i]->createSRVasRenderTexture();
	for (size_t i = 0; i < this->renderCubeMaps.size(); ++i)
		this->renderCubeMaps[i]->createSRVasRenderTexture();
}

void ComputeShader::addShaderBuffer(ShaderBuffer& buffer)
{
	// Add constant buffer, if we haven't reached the maximum yet
	if (this->constantBuffers.size() < NUM_MAX_CONSTANT_BUFFERS)
	{
		this->constantBuffers.push_back(buffer.getBuffer());
	}
	else
		Log::error("Too many shader buffers were attempted to be added for this compute shader.");
}

void ComputeShader::addRenderTexture(Texture& texture)
{
	// Add render texture, if we haven't reached the maximum yet
	if (this->renderTextures.size() < NUM_MAX_RENDER_TEXTURES)
	{
		this->renderTextures.push_back(&texture);
		this->renderTextureUAVs.push_back(texture.getTextureUAV());
	}
	else
		Log::error("Too many render textures were attempted to be added for this compute shader.");
}

void ComputeShader::addRenderCubeMap(CubeMap& cubeMap)
{
	// Add render texture, if we haven't reached the maximum yet
	if (this->renderTextureUAVs.size() < NUM_MAX_RENDER_TEXTURES)
	{
		this->renderCubeMaps.push_back(&cubeMap);
		this->renderTextureUAVs.push_back(cubeMap.getTextureUAV());
	}
	else
		Log::error("Too many render textures were attempted to be added for this compute shader.");
}

void ComputeShader::removeRenderTextureAt(int index)
{
	this->renderTextures.erase(this->renderTextures.begin() + index);
	this->renderTextureUAVs.erase(this->renderTextureUAVs.begin() + index);
}

void ComputeShader::removeAllRenderTextures()
{
	this->renderTextures.clear();
	this->renderTextureUAVs.clear();
}