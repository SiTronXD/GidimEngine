
cbuffer HeightToNormalBuffer : register(b0)
{
	int gridWidth;
	int gridHeight;
	int normalMapWidth;
	int normalMapHeight;

	float unitLength;

	int padding[3];
};

RWTexture2D<float4> heightmapTexture : register(u0);
RWTexture2D<float4> normalMapTexture : register(u1);

float noise(float2 n)
{
	float decimalScale = 10000.0;
	n = round(n * decimalScale) / decimalScale;

	n = frac(n * float2(573.721, 853.712));
	n += dot(n, n + 64.853);

	return frac(n.x * n.y);
}

float perlinNoiseLayer(float2 p)
{
	float offsetStep = 1.0;
	float gridSize = 10.0;

	float2 st = smoothstep(0.0, 1.0, frac(p * gridSize));
	float2 id = floor(p * gridSize);

	float upperLeft = noise(id + float2(0.0, 0.0));
	float upperRight = noise(id + float2(offsetStep, 0.0));
	float lowerLeft = noise(id + float2(0.0, offsetStep));
	float lowerRight = noise(id + float2(offsetStep, offsetStep));

	float upperMix = lerp(upperLeft, upperRight, st.x);
	float lowerMix = lerp(lowerLeft, lowerRight, st.x);

	float finalMix = lerp(upperMix, lowerMix, st.y);

	return finalMix;
}

float perlinNoise(float2 p)
{
	float currentNoise = 0.0;

	currentNoise += perlinNoiseLayer(p * 4.0) / 2.0;
	currentNoise += perlinNoiseLayer(p * 8.0) / 4.0;
	currentNoise += perlinNoiseLayer(p * 16.0) / 8.0;
	currentNoise += perlinNoiseLayer(p * 32.0) / 16.0;
	currentNoise += perlinNoiseLayer(p * 64.0) / 32.0;

	// Returns value from 0 to 0.96875    
	return currentNoise;
}

// Repeat position within grid
uint2 repPos(uint2 pos)
{
	pos += uint2(gridWidth, gridHeight);
	pos.x = pos.x % gridWidth;
	pos.y = pos.y % gridHeight;

	return pos;
}

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

		displacements[i] = heightmapTexture[pos].xyz;
	}

	// Calculate normal from slopes
	float3 horizontalSlope = (float3(unitLength, 0, 0) + displacements[2]) - (float3(-unitLength, 0, 0) + displacements[1]);
	float3 verticalSlope = (float3(0, 0, unitLength) + displacements[0]) - (float3(0, 0, -unitLength) + displacements[3]);
	float3 normal = cross(verticalSlope, horizontalSlope);
	//normal.y *= 0.1;
	normal = normalize(normal);

	return normal;
}

// Bilinearly interpolate between normals 
// (used if normalmap and heightmap have different resolutions)
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

	st.x = smoothstep(0.0, 1.0, st.x);
	st.y = smoothstep(0.0, 1.0, st.y);

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