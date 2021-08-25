// This optimization implementation is based on this blog post:
// Turánszki János. "GPU Fluid Simulation". (https://wickedengine.net/2018/05/21/scalabe-gpu-fluid-simulation/, accessed August 22, 2021)

// The optimization creates a dynamic hashed grid in 3 steps:
// 1. Create unsorted list where each element is (cell ID, boid ID)
// 2. Sort the list based on the cell IDs
// 3. Create offset list that describes cell ID offsets in the sorted list

// The dynamic hashed grid is then used to find neighboring boids in the grid

#define THREAD_GROUP_SIZE 1024

cbuffer BoidInsertBuffer : register(b0)
{
	float halfVolumeSize;
	float maxSearchRadius;

	float2 padding;
};

// uint2(cell ID, boid ID)
RWStructuredBuffer<uint2> boidList : register(u0);
RWStructuredBuffer<float4x4> boidTransforms : register(u1);

// Extract position from transformation matrix
float3 getPos(uint boidIndex)
{
	return float3(
		boidTransforms[boidIndex]._41,
		boidTransforms[boidIndex]._42,
		boidTransforms[boidIndex]._43
	);
}

// Get cell ID that the boid resides within
uint getCellID(uint id)
{
	float3 pos = getPos(id);

	// From [-halfVolumeSize, halfVolumeSize] to [0.0, volumeSize]
	pos += float3(halfVolumeSize, halfVolumeSize, halfVolumeSize);

	// From [0, volumeSize] to [0, maxNumCells - 1]
	pos = floor(pos / maxSearchRadius);

	float maxNumCells = halfVolumeSize * 2.0f / maxSearchRadius;

	return uint(pos.x + pos.y * maxNumCells + pos.z * maxNumCells * maxNumCells);
}

[numthreads(THREAD_GROUP_SIZE, 1, 1)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
	uint id = dispatchThreadID.x;

	// uint2(cell ID, boid ID)
	boidList[id] = uint2(getCellID(id), id);
}