#include "Skybox.h"
#include "../Engine/Application/Time.h"

Skybox::Skybox(Renderer& renderer)
	: meshData(DefaultMesh::CUBE, 0, 0, true), mesh(renderer, this->meshData),
	shader(renderer), shaderBuffer(renderer, sizeof(SkyboxBuffer)), renderer(renderer),
	timer(0.0f), skyCubeMap(renderer), 
	preethamCreatorShader(renderer, CUBE_FACE_WIDTH / 16, CUBE_FACE_HEIGHT / 16, 6 / 2)
{
	this->mesh.setWorldMatrix(XMMatrixScaling(1000.0f, 1000.0f, 1000.0f));

	// Create cube map as render texture
	this->skyCubeMap.createAsRenderTexture(CUBE_FACE_WIDTH, CUBE_FACE_HEIGHT);

	// Preetham creator shader
	this->preethamCreatorShader.createFromFile("CompiledShaders/PreethamSkyCreator_Comp.cso");
	this->preethamCreatorShader.addRenderTexture(this->skyCubeMap);
	this->preethamCreatorShader.addShaderBuffer(this->shaderBuffer);

	// Set constants in shader buffer struct
	this->sb.faceWidth = CUBE_FACE_WIDTH;
	this->sb.faceHeight = CUBE_FACE_HEIGHT;
}

Skybox::~Skybox()
{
}

void Skybox::draw()
{
	// Update timer
	timer += Time::getDeltaTime() * 0.2f;

	// Update and set shader buffer
	this->sb.sunDir = XMFLOAT3(0.0f, cos(timer), sin(timer));
	this->sb.turbidity = 2.0f;
	this->shaderBuffer.update(&this->sb);

	// Render sky box
	this->preethamCreatorShader.run();

	// Set sky box
	this->skyCubeMap.setPS();

	// Update shader
	this->shader.update(renderer, this->mesh.getWorldMatrix());

	// Set shader to render mesh with
	this->shader.set();

	// Render mesh
	this->mesh.draw();
}

CubeMap& Skybox::getCubeMap()
{
	return this->skyCubeMap;
}
