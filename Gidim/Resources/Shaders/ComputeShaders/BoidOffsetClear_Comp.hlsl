
cbuffer BoidOffsetClearBuffer : register(b0)
{
	uint numGridCells;

	float3 padding;
};

RWStructuredBuffer<uint> boidOffsets : register(u0);

[numthreads(32, 1, 1)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
	uint id = dispatchThreadID.x;

	// Make sure thread ID does not overshoot
	if (id < numGridCells)
	{
		boidOffsets[id] = 0xFFFFFFFF;
	}
}