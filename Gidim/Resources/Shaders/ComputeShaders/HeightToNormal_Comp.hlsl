
cbuffer HeightToNormalBuffer : register(b0)
{
	int gridWidth;
	int gridHeight;

	float unitLength;

	int padding;
};

RWTexture2D<float4> heightmapTexture : register(u0);
RWTexture2D<float4> normalMapTexture : register(u1);

[numthreads(16, 16, 1)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
	float3 displacements[4];

	// Find neighboring displacements
	for (int i = 0; i < 4; ++i)
	{
		int j = i * 2 + 1;
		int x = j % 3;
		int y = j / 3;

		// Keep position within the texture, and avoid negative positions
		uint2 pos = uint2(gridWidth, gridHeight) +
			dispatchThreadID.xy + uint2(x, y);
		pos.x = pos.x % gridWidth;
		pos.y = pos.y % gridHeight;

		displacements[i] = heightmapTexture[pos].xyz;
	}

	// Calculate normal from slopes
	float3 horizontalSlope = (float3(unitLength, 0, 0) + displacements[2]) - (float3(-unitLength, 0, 0) + displacements[1]);
	float3 verticalSlope = (float3(0, 0, unitLength) + displacements[0]) - (float3(0, 0, -unitLength) + displacements[3]);
	float3 normal = cross(verticalSlope, horizontalSlope);
	//normal.y *= 0.1;
	normal = normalize(normal);

	normalMapTexture[dispatchThreadID.xy] = 
		float4(
			(normal + 1.0) * 0.5,
			1.0
		);
}