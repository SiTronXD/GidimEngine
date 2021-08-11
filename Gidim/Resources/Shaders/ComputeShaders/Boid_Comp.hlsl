

RWStructuredBuffer<float3> boidsOutput : register(u0);

[numthreads(1, 1, 1)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
	uint id = dispatchThreadID.x;/* +
		dispatchThreadID.y * 1024 + 
		dispatchThreadID.z * 1024 * 1024;*/

	boidsOutput[id] = boidsOutput[id] + float3(0.001, 0.0, 0.0);
}