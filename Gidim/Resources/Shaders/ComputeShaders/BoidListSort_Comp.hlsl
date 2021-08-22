
#define N_ELEMENTS 16

// uint2(cell ID, boid ID)
RWStructuredBuffer<uint2> boidList : register(u0);

groupshared uint2 localValue[N_ELEMENTS];

void localCompareAndSwap(int2 idx)
{
	if (localValue[idx.x].x > localValue[idx.y].x)
	{
		uint2 temp = localValue[idx.x];
		localValue[idx.x] = localValue[idx.y];
		localValue[idx.y] = temp;
	}
}

void doFlip(uint t, int h)
{
	int q = int((2 * t) / h) * h;
	int halfH = h / 2;
	int2 indices = int2(q, q) + int2(t % halfH, h - (t % halfH) - 1);
	localCompareAndSwap(indices);
}

void doDisperse(uint t, int h)
{
	int q = int((2 * t) / h) * h;
	int halfH = h / 2;
	int2 indices = int2(q, q) + int2(t % halfH, (t % halfH) + halfH);
	localCompareAndSwap(indices);
}

[numthreads(N_ELEMENTS / 2, 1, 1)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
	uint t = dispatchThreadID.x;

	localValue[t * 2 + 0] = boidList[t * 2 + 0];
	localValue[t * 2 + 1] = boidList[t * 2 + 1];

	int n = N_ELEMENTS;

	for (uint h = 2; h <= n; h *= 2)
	{
		GroupMemoryBarrierWithGroupSync();
		doFlip(t, h);

		for (uint hh = h / 2; hh > 1; hh /= 2)
		{
			GroupMemoryBarrierWithGroupSync();
			doDisperse(t, hh);
		}
	}

	GroupMemoryBarrierWithGroupSync();


	boidList[t * 2 + 0] = localValue[t * 2 + 0];
	boidList[t * 2 + 1] = localValue[t * 2 + 1];
}