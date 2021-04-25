#include "ComputeShader.h"
#include <fstream>

ComputeShader::ComputeShader(int threadGroupX, int threadGroupY)
	: threadGroupX(threadGroupX), threadGroupY(threadGroupY),
	computeShader(nullptr), uavNULL{ NULL }
{ }

ComputeShader::~ComputeShader()
{
	S_RELEASE(this->computeShader);
}

bool ComputeShader::createFromFile(Renderer& renderer, std::string path)
{
	// Deallocate old texture, if it exists
	S_RELEASE(this->computeShader);

	// Only accept shader model 5.0 for best performance, and ignore
	// shader model 4.0
	if (renderer.getDevice()->GetFeatureLevel() < D3D_FEATURE_LEVEL_11_0)
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
	HRESULT result = renderer.getDevice()->CreateComputeShader(
		csData.data(), csData.size(), nullptr, &this->computeShader
	);
	if (FAILED(result))
	{
		Log::error("Failed calling device->CreateComputerShader().");

		return false;
	}

	return true;
}

void ComputeShader::run(Renderer& renderer)
{
	ID3D11DeviceContext* deviceContext = renderer.getDeviceContext();

	// Set
	deviceContext->CSSetShader(this->computeShader, NULL, 0);
	deviceContext->CSSetUnorderedAccessViews(
		0, this->renderTextureUAVs.size(), &this->renderTextureUAVs[0], NULL
	);

	// Run
	deviceContext->Dispatch(this->threadGroupX, this->threadGroupY, 1);

	// Reset
	deviceContext->CSSetShader(NULL, NULL, 0);
	deviceContext->CSSetUnorderedAccessViews(0, NUM_MAX_RENDER_TEXTURES, uavNULL, NULL);


	// Recreate SRVs after dispatch
	for(int i = 0; i < this->renderTextures.size(); ++i)
		this->renderTextures[i]->recreateSRVAsRenderTexture(renderer);
}

void ComputeShader::addRenderTexture(Texture& texture)
{
	// Add render texture, if we haven't reached the maximum yet
	if (this->renderTextures.size() < NUM_MAX_RENDER_TEXTURES)
	{
		this->renderTextures.push_back(&texture);
		this->renderTextureUAVs.push_back(texture.getTextureUAV());
	}
}
