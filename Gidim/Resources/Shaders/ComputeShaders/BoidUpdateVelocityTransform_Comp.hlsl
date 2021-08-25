
#define THREAD_GROUP_SIZE 1024

RWStructuredBuffer<float3> boidVelocities : register(u0);
RWStructuredBuffer<float3> boidNewVelocities : register(u1);
RWStructuredBuffer<float4x4> boidTransforms : register(u2);
RWStructuredBuffer<float4x4> boidNewTransforms : register(u3);

[numthreads(THREAD_GROUP_SIZE, 1, 1)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
	uint id = dispatchThreadID.x;

	boidVelocities[id] = boidNewVelocities[id];
	boidTransforms[id] = boidNewTransforms[id];
}