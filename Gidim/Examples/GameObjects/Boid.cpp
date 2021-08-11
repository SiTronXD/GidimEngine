#include "../../pch.h"
#include "Boid.h"

Boid::Boid(Renderer& renderer)
	: renderer(renderer), shader(renderer), 
	meshData(DefaultMesh::TETRAHEDRON, 1, 1), 
	mesh(renderer, meshData)
{
}

Boid::~Boid()
{
}

void Boid::draw()
{
	this->shader.update(this->renderer, this->mesh.getWorldMatrix());

	this->shader.set();

	this->mesh.draw();
}