
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

[numthreads(2, 1, 1)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
	uint id = dispatchThreadID.x;

	// uint2(cell ID, boid ID)
	boidList[id] = uint2(getCellID(id), id);
}