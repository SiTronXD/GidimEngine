
#define THREAD_GROUP_SIZE 8

RWStructuredBuffer<uint2> boidList : register(u0);
RWStructuredBuffer<uint> boidOffsets : register(u1);

[numthreads(THREAD_GROUP_SIZE, 1, 1)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
	uint id = dispatchThreadID.x;
	uint boidCellID = boidList[id].x;

	InterlockedMin(boidOffsets[boidCellID], id);
}