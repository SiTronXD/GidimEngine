

RWStructuredBuffer<float3> boidsOutput : register(u0);

[numthreads(2, 1, 1)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
	uint id = dispatchThreadID.x;/* +
		dispatchThreadID.y * 1024 + 
		dispatchThreadID.z * 1024 * 1024;*/

	float3 offset = float3(0.0002, 0.0, 0.0);

	if (id >= 1)
		offset = float3(-0.0003, 0.0001, 0.0);

	boidsOutput[id] = boidsOutput[id] + offset;
}