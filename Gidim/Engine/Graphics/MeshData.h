#pragma once

#include "../../pch.h"

struct Vertex
{
	float x, y, z;
	float u, v;
};

enum class DefaultMesh
{
	TRIANGLE,
	TETRAHEDRON,
	CUBE,
	
	PLANE,
	SPHERE
};

class MeshData
{
private:
	std::vector<Vertex> vertices;
	std::vector<int> indices;

	void removeOldData();

	void createTriangle();
	void createTetrahedron();
	void createCube();
	void createPlane(int resX, int resY);
	void createSphere(int resX, int resY);

	Vertex makeVert(float _x, float _y, float _z, float _u, float _v);

public:
	MeshData();
	MeshData(std::vector<Vertex>& vertices, std::vector<int>& indices);
	MeshData(
		DefaultMesh defaultMeshType, int resolutionX, int resolutionY, 
		bool shouldInvertFaces = false
	);

	void createDefault(DefaultMesh defaultMeshType, int resolutionX = 0, int resolutionY = 0);
	void invertFaces();

	std::vector<Vertex>& getVertices();
	std::vector<int>& getIndices();
};