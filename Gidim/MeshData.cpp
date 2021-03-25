#include "MeshData.h"

void MeshData::removeOldData()
{
	this->vertices.resize(0);
	this->indices.resize(0);
}

void MeshData::createTriangle()
{
	// Vertices
	this->vertices.push_back(this->makeVert( 1.0f,	0.0f,	0.0f,		0.0f, 1.0f ));
	this->vertices.push_back(this->makeVert( 0.0f,	1.0f,	0.0f,		0.5f, 0.0f ));
	this->vertices.push_back(this->makeVert(-1.0f,	0.0f,	0.0f,		1.0f, 1.0f ));

	// Indices
	for (int i = 0; i < 3; ++i)
		this->indices.push_back(i);
}

void MeshData::createTetrahedron()
{
	// Vertices
	this->vertices.push_back(this->makeVert( -0.866f,	0.0f,	 0.5f,		0.0f, 1.0f ));
	this->vertices.push_back(this->makeVert(  0.0f,		0.0f,	-1.0f,		0.5f, 0.0f ));
	this->vertices.push_back(this->makeVert(  0.866f,	0.0f,	 0.5f,		1.0f, 1.0f ));
							 
	this->vertices.push_back(this->makeVert(  0.0f,		0.0f,	-1.0f,		1.0f, 1.0f ));
	this->vertices.push_back(this->makeVert( -0.866f,	0.0f,	 0.5f,		0.0f, 1.0f ));
	this->vertices.push_back(this->makeVert(  0.0f,		1.5f,	 0.0f,		0.5f, 0.0f ));
							 
	this->vertices.push_back(this->makeVert(  0.866f,	0.0f,	 0.5f,		1.0f, 1.0f ));
	this->vertices.push_back(this->makeVert(  0.0f,		0.0f,	-1.0f,		0.0f, 1.0f ));
	this->vertices.push_back(this->makeVert(  0.0f,		1.5f,	 0.0f,		0.5f, 0.0f ));

	this->vertices.push_back(this->makeVert( -0.866f,	0.0f,	 0.5f,		1.0f, 1.0f ));
	this->vertices.push_back(this->makeVert(  0.866f,	0.0f,	 0.5f,		0.0f, 1.0f ));
	this->vertices.push_back(this->makeVert(  0.0f,		1.5f,	 0.0f,		0.5f, 0.0f ));


	// Indices
	for(int i = 0; i < 12; ++i)
		this->indices.push_back(i);
}

void MeshData::createCube(bool invertFaces)
{
	// Vertices

	// Front
	this->vertices.push_back(this->makeVert( 0.5f, -0.5f,  0.5f,	0.0f, 1.0f));
	this->vertices.push_back(this->makeVert( 0.5f,  0.5f,  0.5f,	0.0f, 0.0f));
	this->vertices.push_back(this->makeVert(-0.5f, -0.5f,  0.5f,	1.0f, 1.0f));
	this->vertices.push_back(this->makeVert(-0.5f,  0.5f,  0.5f,	1.0f, 0.0f));

	// Back
	this->vertices.push_back(this->makeVert( 0.5f, -0.5f, -0.5f,	1.0f, 1.0f));
	this->vertices.push_back(this->makeVert(-0.5f, -0.5f, -0.5f,	0.0f, 1.0f));
	this->vertices.push_back(this->makeVert( 0.5f,  0.5f, -0.5f,	1.0f, 0.0f));
	this->vertices.push_back(this->makeVert(-0.5f,  0.5f, -0.5f,	0.0f, 0.0f));

	// Top
	this->vertices.push_back(this->makeVert( 0.5f,  0.5f, -0.5f,	1.0f, 1.0f));
	this->vertices.push_back(this->makeVert(-0.5f,  0.5f, -0.5f,	0.0f, 1.0f));
	this->vertices.push_back(this->makeVert( 0.5f,  0.5f,  0.5f,	1.0f, 0.0f));
	this->vertices.push_back(this->makeVert(-0.5f,  0.5f,  0.5f,	0.0f, 0.0f));

	// Bottom
	this->vertices.push_back(this->makeVert( 0.5f, -0.5f, -0.5f,	0.0f, 1.0f));
	this->vertices.push_back(this->makeVert( 0.5f, -0.5f,  0.5f,	0.0f, 0.0f));
	this->vertices.push_back(this->makeVert(-0.5f, -0.5f, -0.5f,	1.0f, 1.0f));
	this->vertices.push_back(this->makeVert(-0.5f, -0.5f,  0.5f,	1.0f, 0.0f));

	// Left
	this->vertices.push_back(this->makeVert(0.5f,  0.5f,  -0.5f,	0.0f, 0.0f));
	this->vertices.push_back(this->makeVert(0.5f,  0.5f,   0.5f,	1.0f, 0.0f));
	this->vertices.push_back(this->makeVert(0.5f, -0.5f,  -0.5f,	0.0f, 1.0f));
	this->vertices.push_back(this->makeVert(0.5f, -0.5f,   0.5f,	1.0f, 1.0f));
												  
	// Right
	this->vertices.push_back(this->makeVert(-0.5f,  0.5f, -0.5f,	1.0f, 0.0f));
	this->vertices.push_back(this->makeVert(-0.5f, -0.5f, -0.5f,	1.0f, 1.0f));
	this->vertices.push_back(this->makeVert(-0.5f,  0.5f,  0.5f,	0.0f, 0.0f));
	this->vertices.push_back(this->makeVert(-0.5f, -0.5f,  0.5f,	0.0f, 1.0f));

	// Indices
	for (int i = 0; i < 6; ++i)
	{
		// Triangle 1
		this->indices.push_back(i * 4 + 0);
		this->indices.push_back(i * 4 + 1);
		this->indices.push_back(i * 4 + 2);

		// Triangle 2
		this->indices.push_back(i * 4 + 1);
		this->indices.push_back(i * 4 + 3);
		this->indices.push_back(i * 4 + 2);
	}
}

void MeshData::createPlane(int resX, int resY)
{
	// Default resolution
	if (resX <= 1 || resY <= 1)
	{
		resX = 10;
		resY = 10;
	}
	
	// Vertices
	for (int y = 0; y < resY; ++y)
	{
		for (int x = 0; x < resX; ++x)
		{
			this->vertices.push_back(
				this->makeVert(
					((float) x / (resX - 1)) - 0.5f, 
					0, 
					((float) -y / (resY - 1)) + 0.5f, 

					(float) x / (resX - 1), 
					(float) y / (resY - 1)
				)
			);
		}
	}

	// Indices
	for (int i = 0; i < (resX - 1) * (resY - 1); ++i)
	{
		int squareX = i % (resX - 1);
		int squareY = i / (resX - 1);

		this->indices.push_back(squareY * resX + squareX + 0);
		this->indices.push_back(squareY * resX + squareX + resX + 1);
		this->indices.push_back(squareY * resX + squareX + resX);

		this->indices.push_back(squareY * resX + squareX + 0);
		this->indices.push_back(squareY * resX + squareX + 1);
		this->indices.push_back(squareY * resX + squareX + resX + 1);
	}
}

void MeshData::createSphere(int resX, int resY)
{
	// Default resolution
	if (resX <= 2 || resY <= 2)
	{
		resX = 10;
		resY = 10;
	}

	const float PI = 3.1415f;

	// Vertices
	for (int y = 0; y < resY; ++y)
	{
		float ty = (float) y / (resY - 1);

		for (int x = 0; x < resX; ++x)
		{
			float angle = (float) x / (resX - 1);

			float vx = sin(angle * 2.0f * PI) * sin(ty * PI);
			float vy = cos((1.0f - ty) * PI);
			float vz = cos(angle * 2.0f * PI) * sin(ty * PI);

			float vu = 1.0f - (float) x / (resX - 1);
			float vv = 1.0f - ty;

			this->vertices.push_back(
				this->makeVert(
					vx, vy, vz, 
					vu,
					vv
				)
			);
		}
	}

	// Indices
	for (int i = 0; i < (resX - 1) * (resY - 1); ++i)
	{
		int squareX = i % (resX - 1);
		int squareY = i / (resX - 1);

		this->indices.push_back(squareY * resX + squareX + 0);
		this->indices.push_back(squareY * resX + squareX + resX + 1);
		this->indices.push_back(squareY * resX + squareX + resX);

		this->indices.push_back(squareY * resX + squareX + 0);
		this->indices.push_back(squareY * resX + squareX + 1);
		this->indices.push_back(squareY * resX + squareX + resX + 1);
	}

	// Create triangles between the 2 vertical edges to avoid 
	// precision errors and z-fighting
	for (int i = 0; i < resY - 1; ++i)
	{
		int squareY = i * resX;

		this->indices.push_back(squareY + resX - 1 + 0);
		this->indices.push_back(squareY + resX - 1 + 1);
		this->indices.push_back(squareY + resX - 1 + resX);

		this->indices.push_back(squareY + resX - 1 + 0);
		this->indices.push_back(squareY + 0);
		this->indices.push_back(squareY + resX + 0);
	}
}

Vertex MeshData::makeVert(float _x, float _y, float _z, float _u, float _v)
{
	return { _x, _y, _z, _u, _v };
}

MeshData::MeshData()
{
}

MeshData::MeshData(std::vector<Vertex>& vertices, std::vector<int>& indices)
	: vertices(vertices), indices(indices)
{ }

void MeshData::createDefault(DefaultMesh defaultMeshType, int resolutionX, int resolutionY)
{
	// Remove earlier vertex info, just to be sure
	this->removeOldData();

	// Create vertex information
	switch (defaultMeshType)
	{
	case DefaultMesh::TRIANGLE:

		this->createTriangle();

		break;

	case DefaultMesh::TETRAHEDRON:

		this->createTetrahedron();

		break;

	case DefaultMesh::CUBE:

		this->createCube(false);

		break;

	case DefaultMesh::PLANE:

		this->createPlane(resolutionX, resolutionY);

		break;

	case DefaultMesh::SPHERE:

		this->createSphere(resolutionX, resolutionY);

		break;
	}
}

void MeshData::invertFaces()
{
	// Swap 2 indices in each triangle
	for (int i = 0; i < this->indices.size(); i += 3)
	{
		int temp = this->indices[i];
		this->indices[i] = this->indices[i + 1];
		this->indices[i + 1] = temp;
	}
}

std::vector<Vertex>& MeshData::getVertices()
{
	return this->vertices;
}

std::vector<int>& MeshData::getIndices()
{
	return this->indices;
}