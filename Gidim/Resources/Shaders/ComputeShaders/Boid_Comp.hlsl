
cbuffer BoidBuffer : register(b0)
{
	float deltaTime;

	float3 padding;
};

RWStructuredBuffer<float3> boidAccelerations : register(u0);
RWStructuredBuffer<float3> boidVelocities : register(u1);
RWStructuredBuffer<float4x4> boidTransforms : register(u2);

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

[numthreads(16, 1, 1)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
	uint id = dispatchThreadID.x;

	// Apply movement
	boidVelocities[id] += boidAccelerations[id] * deltaTime;

	float3 oldPos = float3(boidTransforms[id]._41, boidTransforms[id]._42, boidTransforms[id]._43);
	float3 newPos = oldPos + boidVelocities[id] * deltaTime;

	// Direction vectors
	float3 forwardDir = normalize(boidVelocities[id]);
	float3 leftDir = normalize(cross(forwardDir, float3(0.0f, 1.0f, 0.0f)));
	float3 upDir = cross(leftDir, forwardDir);

	// Apply final matrix
	boidTransforms[id] = float4x4(
		float4(leftDir.xyz, 0.0f),
		float4(upDir.xyz, 0.0f),
		float4(forwardDir.xyz, 0.0f),
		float4(newPos, 1.0f)
	);
}