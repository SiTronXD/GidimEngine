
cbuffer MatrixBuffer : register(b0)
{
	matrix projectionMatrix;
	matrix viewMatrix;
	matrix worldMatrix;
};

struct Input
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
	uint instanceID : SV_InstanceID;
};

struct Output
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
	float3 color : TEXCOORD1;
};

StructuredBuffer<float4x4> boidsBuffer : register(t0);
StructuredBuffer<uint2> boidsList : register(t1);

uint wang_hash(uint seed)
{
	seed = uint(seed ^ uint(61)) ^ uint(seed >> uint(16));
	seed *= uint(9);
	seed = seed ^ (seed >> 4);
	seed *= uint(0x27d4eb2d);
	seed = seed ^ (seed >> 15);

	return seed;
}

float randomFloat(float state)
{
	return float(wang_hash(uint(state))) / 4294967296.0;
}

// Types of coloring for the boids
#define COLOR_TYPE_BOID_ID 0
#define COLOR_TYPE_CELL_ID 1

#define SELECTED_COLOR_TYPE COLOR_TYPE_CELL_ID

Output main(Input input)
{
	Output output;

	uint id = input.instanceID;
	uint colID = id;

	// Choose IDs depending on the coloring type
#if (SELECTED_COLOR_TYPE == COLOR_TYPE_CELL_ID)
	id = boidsList[input.instanceID].y;
	colID = boidsList[input.instanceID].x;
#endif

	// Color
	output.color = float3(
		randomFloat(colID * 3 + 0),
		randomFloat(colID * 3 + 1),
		randomFloat(colID * 3 + 2)
	);

	// Position
	float4 p = float4(input.position.xyz, 1.0f);
	output.position = mul(p, boidsBuffer[id]);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// UV coordinates
	output.uv = input.uv;

	return output;
}