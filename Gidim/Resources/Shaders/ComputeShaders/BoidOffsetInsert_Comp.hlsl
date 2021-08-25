
#define THREAD_GROUP_SIZE 1024

RWStructuredBuffer<uint2> boidSortedList : register(u0);
RWStructuredBuffer<uint> boidListOffsets : register(u1);

[numthreads(THREAD_GROUP_SIZE, 1, 1)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
	uint id = dispatchThreadID.x;
	uint boidCellID = boidSortedList[id].x;

	InterlockedMin(boidListOffsets[boidCellID], id);
}