#include "ComputeShader.h"
#include "../Dev/Helpers.h"

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

ComputeShader::ComputeShader(Renderer& renderer, const std::string path,
	int threadGroupX, int threadGroupY, int threadGroupZ)
	: threadGroupX(threadGroupX), threadGroupY(threadGroupY), threadGroupZ(threadGroupZ),
	computeShader(nullptr), uavNULL{ NULL }, constantBufferNULL{ NULL },
	device(renderer.getDevice()), deviceContext(renderer.getDeviceContext())
{ 
	this->createFromFile(path);
}

ComputeShader::~ComputeShader()
{
	S_RELEASE(this->computeShader);
}

void ComputeShader::run()
{
	// Make sure atleast 1 resource is attached
	if (this->unorderedAccessViews.size() <= 0 && this->renderTextures.size() <= 0 &&
		this->constantBuffers.size() <= 0)
	{
		Log::error("This compute shader has no resources attached to it...");
	}

	// Set
	this->deviceContext->CSSetShader(this->computeShader, NULL, 0);
	this->deviceContext->CSSetUnorderedAccessViews(
		0, this->unorderedAccessViews.size(), VECTOR_ADDRESS(this->unorderedAccessViews), NULL
	);
	this->deviceContext->CSSetConstantBuffers(
		0, this->constantBuffers.size(), VECTOR_ADDRESS(this->constantBuffers)
	);

	// Run
	this->deviceContext->Dispatch(
		this->threadGroupX, 
		this->threadGroupY, 
		this->threadGroupZ
	);

	// Reset
	this->deviceContext->CSSetShader(NULL, NULL, 0);
	this->deviceContext->CSSetUnorderedAccessViews(0, NUM_MAX_UAV, uavNULL, NULL);
	this->deviceContext->CSSetConstantBuffers(0, NUM_MAX_CONSTANT_BUFFERS, constantBufferNULL);

	// Recreate SRVs after dispatch
	for(size_t i = 0; i < this->renderTextures.size(); ++i)
		this->renderTextures[i]->createSRV();
}

void ComputeShader::addConstantBuffer(ConstantBuffer& buffer)
{
	// Add constant buffer, if we haven't reached the maximum yet
	if (this->constantBuffers.size() < NUM_MAX_CONSTANT_BUFFERS)
	{
		this->constantBuffers.push_back(buffer.getBuffer());
	}
	else
		Log::error("Too many shader buffers were attempted to be added for this compute shader.");
}

void ComputeShader::addUAV(ID3D11UnorderedAccessView* uavToAdd)
{
	if (this->unorderedAccessViews.size() < NUM_MAX_UAV)
	{
		this->unorderedAccessViews.push_back(uavToAdd);
	}
	else
		Log::error("Too many UAVs were attempted to be added for this compute shader.");
}

void ComputeShader::addRenderTexture(Texture& texture)
{
	// Add render texture, if we haven't reached the maximum yet
	if (this->renderTextures.size() < NUM_MAX_RENDER_TEXTURES)
	{
		this->renderTextures.push_back(&texture);
		this->addUAV(texture.getTextureUAV());
	}
	else
		Log::error("Too many render textures were attempted to be added for this compute shader.");
}

// This is assuming that all UAVs have corresponding render textures
void ComputeShader::removeRenderTextureAt(int index)
{
	this->renderTextures.erase(this->renderTextures.begin() + index);
	this->unorderedAccessViews.erase(this->unorderedAccessViews.begin() + index);
}

void ComputeShader::removeAllRenderTextures()
{
	this->renderTextures.clear();
	this->unorderedAccessViews.clear();
}