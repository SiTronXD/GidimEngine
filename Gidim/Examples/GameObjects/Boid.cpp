#include "../../pch.h"
#include "Boid.h"

Boid::Boid(Renderer& renderer)
	: renderer(renderer), shader(renderer), 
	meshData(
		DefaultMesh::TETRAHEDRON, 1, 1, false, 
		XMMatrixMultiply(
			XMMatrixScaling(0.2f, 0.4f, 0.2f), 
			XMMatrixRotationX(XM_PI * 0.5f)
		)
	), 
	mesh(renderer, meshData)
{
}

Boid::~Boid()
{
}

void Boid::draw()
{
	this->shader.update(this->renderer, XMMatrixIdentity());

	this->shader.set();

	this->mesh.draw(false);
}