#include "Skybox.h"
#include "../Engine/Application/Time.h"

Skybox::Skybox(Renderer& renderer)
	: meshData(DefaultMesh::CUBE, 0, 0, true), mesh(renderer, this->meshData),
	shader(renderer), shaderBuffer(renderer, sizeof(SkyboxBuffer)), renderer(renderer),
	timer(0.0f)
{
	this->mesh.setWorldMatrix(XMMatrixScaling(1000.0f, 1000.0f, 1000.0f));
}

Skybox::~Skybox()
{
}

void Skybox::draw()
{
	// Update timer
	timer += Time::getDeltaTime() * 0.2f;

	// Update buffers in shader
	this->shader.update(renderer, this->mesh.getWorldMatrix());

	// Update and set shader buffer
	this->sb.sunDir[0] = 0.0f;
	this->sb.sunDir[1] = cos(timer);
	this->sb.sunDir[2] = sin(timer);
	this->sb.turbidity = 2.0f;
	this->shaderBuffer.update(&this->sb);
	this->shaderBuffer.setPS();

	// Set shader to render mesh with
	this->shader.set();

	// Render mesh
	this->mesh.draw();
}
