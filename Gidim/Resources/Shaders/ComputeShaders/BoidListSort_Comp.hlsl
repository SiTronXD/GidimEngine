
// This sorting implementation is based on this blog post:
// Tim Gfrerer. "Implementing Bitonic Merge Sort in Vulkan Compute". (https://poniesandlight.co.uk/reflect/bitonic_merge_sort/, accessed August 22, 2021)

// Each thread controls a pair of elements, hence size/2
#define DESIRED_THREAD_GROUP_SIZE 1024
#define LOCAL_SIZE (DESIRED_THREAD_GROUP_SIZE / 2)

#define ALG_LOCAL_BMS 0
#define ALG_LOCAL_DISPERSE 1
#define ALG_BIG_FLIP 2
#define ALG_BIG_DISPERSE 3

cbuffer BoidSortBuffer : register(b0)
{
	int numElements;
	int subAlgorithmEnum;
	int parameterH;

	float padding;
};

// uint2(cell ID, boid ID)
// (sort elements by cell IDs)
RWStructuredBuffer<uint2> boidList : register(u0);

groupshared uint2 localValue[LOCAL_SIZE * 2];

// Compare and swap globally within dispatch
void globalCompareAndSwap(uint2 idx)
{
	if (boidList[idx.x].x > boidList[idx.y].x)
	{
		uint2 temp = boidList[idx.x];
		boidList[idx.x] = boidList[idx.y];
		boidList[idx.y] = temp;
	}
}

// Compare and swap within thread group
void localCompareAndSwap(uint2 idx)
{
	if (localValue[idx.x].x > localValue[idx.y].x)
	{
		uint2 temp = localValue[idx.x];
		localValue[idx.x] = localValue[idx.y];
		localValue[idx.y] = temp;
	}
}

// Flip globally within dispatch
void bigFlip(uint globalInvocationID, uint workGroupSize, uint h)
{
	uint t = globalInvocationID;
	uint halfH = h >> 1;
	uint tModHH = t % halfH;
	uint q = uint((2 * t) / h) * h;

	uint2 indices = uint2(
		q + tModHH,
		q + h - tModHH - 1
	);

	globalCompareAndSwap(indices);
}

// Disperse globally within dispatch
void bigDisperse(uint globalInvocationID, uint workGroupSize, uint h)
{
	uint t = globalInvocationID;
	uint halfH = h >> 1;
	uint tModHH = t % halfH;
	uint q = uint((2 * t) / h) * h;

	uint2 indices = uint2(
		q + tModHH,
		q + tModHH + halfH
	);

	globalCompareAndSwap(indices);
}

// Flip within thread group
void localFlip(uint localID, uint h)
{
	uint t = localID;

	uint halfH = h >> 1;
	uint tModHH = t % halfH;
	uint q = uint((2 * t) / h) * h;

	uint2 indices = uint2(
		q + tModHH,
		q + h - tModHH - 1
	);

	// Sync threads within group
	GroupMemoryBarrierWithGroupSync();

	localCompareAndSwap(indices);
}

// Disperse within thread group
void localDisperse(uint localID, uint h)
{
	uint t = localID;

	for ( ; h > 1; h >>= 1)
	{
		uint halfH = h >> 1;
		uint tModHH = t % halfH;
		uint q = uint((2 * t) / h) * h;

		uint2 indices = uint2(
			q + tModHH,
			q + tModHH + halfH
		);

		// Sync threads within group
		GroupMemoryBarrierWithGroupSync();

		localCompareAndSwap(indices);
	}
}

// "Regular" Bitonic mergesort within thread group
void localBMS(uint localID, uint h)
{
	for (uint hh = 2; hh <= h; hh <<= 1)
	{
		localFlip(localID, hh);
		localDisperse(localID, hh >> 1);
	}
}

[numthreads(LOCAL_SIZE, 1, 1)]
void main(
	uint3 dispatchThreadID : SV_DispatchThreadID,
	uint3 localThreadID : SV_GroupThreadID,
	uint3 groupID : SV_GroupID
)
{
	uint localID = localThreadID.x;
	uint offset = LOCAL_SIZE * 2 * groupID.x;

	// Copy list values into shared memory for local use
	if (subAlgorithmEnum <= ALG_LOCAL_DISPERSE)
	{
		localValue[localID * 2 + 0] = boidList[offset + localID * 2 + 0];
		localValue[localID * 2 + 1] = boidList[offset + localID * 2 + 1];
	}

	// Choose sub-algorithm
	switch (subAlgorithmEnum)
	{
	case ALG_LOCAL_BMS:
		localBMS(localID, parameterH);
		break;

	case ALG_LOCAL_DISPERSE:
		localDisperse(localID, parameterH);
		break;

	case ALG_BIG_FLIP:
		bigFlip(dispatchThreadID.x, LOCAL_SIZE, parameterH);
		break;

	case ALG_BIG_DISPERSE:
		bigDisperse(dispatchThreadID.x, LOCAL_SIZE, parameterH);
		break;
	}

	// Copy shared memory into list values after local use
	if (subAlgorithmEnum <= ALG_LOCAL_DISPERSE)
	{
		GroupMemoryBarrierWithGroupSync();

		boidList[offset + localID * 2 + 0] = localValue[localID * 2 + 0];
		boidList[offset + localID * 2 + 1] = localValue[localID * 2 + 1];
	}
}