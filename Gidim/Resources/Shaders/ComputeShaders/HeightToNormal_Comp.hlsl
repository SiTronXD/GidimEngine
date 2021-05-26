
cbuffer HeightToNormalBuffer : register(b0)
{
	int gridWidth;
	int gridHeight;

	int padding[2];
};

RWTexture2D<float4> heightmapTexture : register(u0);
RWTexture2D<float4> normalMapTexture : register(u1);

[numthreads(16, 16, 1)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
	float heights[9];

	for (int y = -1; y <= 1; ++y)
	{
		for (int x = -1; x <= 1; ++x)
		{
			// Keep position within the texture, and avoid negative positions
			uint2 pos = uint2(gridWidth, gridHeight) +
				dispatchThreadID.xy + uint2(x, y);
			pos.x = pos.x % gridWidth;
			pos.y = pos.y % gridHeight;

			heights[uint(x + 1) + uint(y + 1) * 3] = heightmapTexture[pos].g;
		}
	}

	float horizontalSlope = (heights[3] - heights[5]) * 0.5;
	float verticalSlope = (heights[1] - heights[7]) * 0.5;
	float upSlope = 0.03;
	float3 normal = normalize(float3(horizontalSlope, upSlope, verticalSlope));

	normalMapTexture[dispatchThreadID.xy] = 
		float4(
			(normal + 1.0) * 0.5,
			1.0
		);
}