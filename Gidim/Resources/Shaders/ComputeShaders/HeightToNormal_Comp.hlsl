
cbuffer HeightToNormalBuffer : register(b0)
{
	int gridWidth;
	int gridHeight;
	int normalMapWidth;
	int normalMapHeight;

	float unitLength;

	int padding[3];
};

RWTexture2D<float4> displacementMapTexture : register(u0);
RWTexture2D<float4> normalMapTexture : register(u1);


// Repeat position within grid
uint2 repPos(uint2 pos)
{
	pos += uint2(gridWidth, gridHeight);
	pos.x = pos.x % gridWidth;
	pos.y = pos.y % gridHeight;

	return pos;
}

// Get position from position in normalmap
// to position in displacementmap
uint2 getGridPos(uint2 dispatchThreadID)
{
	float2 factor = float2(gridWidth, gridHeight) / float2(normalMapWidth, normalMapHeight);
	uint2 result = uint2(float2(dispatchThreadID) * factor);

	return result;
}

float3 calcNormal(uint2 dispatchThreadID)
{
	float3 displacements[4];

	// Find neighboring displacements
	for (int i = 0; i < 4; ++i)
	{
		int j = i * 2 + 1;
		int x = j % 3;
		int y = j / 3;

		// Keep position within the texture, and avoid negative positions
		uint2 pos = repPos(dispatchThreadID + uint2(x, y));

		displacements[i] = displacementMapTexture[pos].xyz;
	}

	// Calculate normal from slopes
	float3 horizontalSlope = (float3(unitLength, 0, 0) + displacements[2]) - (float3(-unitLength, 0, 0) + displacements[1]);
	float3 verticalSlope = (float3(0, 0, unitLength) + displacements[0]) - (float3(0, 0, -unitLength) + displacements[3]);
	float3 normal = cross(verticalSlope, horizontalSlope);
	normal = normalize(normal);

	return normal;
}

// Bilinearly interpolate between normals 
// (use if normalmap and displacementmap have different resolutions)
float3 bilinearInterpolatedNormal(uint2 dispatchThreadID)
{
	// Normal neighbors
	float3 normalA = calcNormal(getGridPos(dispatchThreadID) + uint2(0, 0));
	float3 normalB = calcNormal(getGridPos(dispatchThreadID) + uint2(1, 0));
	float3 normalC = calcNormal(getGridPos(dispatchThreadID) + uint2(0, 1));
	float3 normalD = calcNormal(getGridPos(dispatchThreadID) + uint2(1, 1));

	// Local neighbor coordinates
	float2 st = dispatchThreadID.xy * float2(gridWidth, gridHeight) /
		float2(normalMapWidth, normalMapHeight);
	st = frac(st);

	// Interpolate
	float3 normal = lerp(
		lerp(normalA, normalB, st.x),
		lerp(normalC, normalD, st.x),
		st.y
	);
}

[numthreads(16, 16, 1)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
	float3 normal = calcNormal(getGridPos(dispatchThreadID));
	
	normalMapTexture[dispatchThreadID.xy] = 
		float4(
			(normal + 1.0) * 0.5,
			0.0
		);
}