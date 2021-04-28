
cbuffer InvPermBuffer : register(b0)
{
	int pingPong;
	int padding1;
	int padding2;
	int padding3;
};

RWTexture2D<float4> displacementTexture : register(u0);
RWTexture2D<float4> pingPong0 : register(u1);
RWTexture2D<float4> pingPong1 : register(u2);


#define _PI 3.14159265

struct complex
{
	float realNum;
	float imgNum;
};

complex createComplex(float value1, float value2)
{
	complex createdComplex;
	createdComplex.realNum = value1;
	createdComplex.imgNum = value2;

	return createdComplex;
}

complex createComplex(float2 values)
{
	return createComplex(values.x, values.y);
}

complex complexAdd(complex val1, complex val2)
{
	return createComplex(val1.realNum + val2.realNum, val1.imgNum + val2.imgNum);
}

complex complexMul(complex val1, complex val2)
{
	// (a + bi)(c + di) = (ac - bd) + i(ad + bc)
	complex createdComplex;
	createdComplex.realNum = (val1.realNum * val2.realNum) - (val1.imgNum * val2.imgNum);
	createdComplex.imgNum = (val1.realNum * val2.imgNum) + (val1.imgNum * val2.realNum);

	return createdComplex;
}


[numthreads(16, 16, 1)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
	int N = 256;

	uint2 pos = dispatchThreadID.xy;

	float perms[] = { 1.0, -1.0 };
	int index = int(pos.x + pos.y) % 2;
	float curPerm = perms[index];

	float h = 0.0;

	if (pingPong == 0)
	{
		h = pingPong0[pos].r;
	}
	else if (pingPong == 1)
	{
		h = pingPong1[pos].r;
	}

	float finalVal = curPerm * h / (float(N) * float(N));

	displacementTexture[pos] = float4(
		finalVal,
		finalVal,
		finalVal,
		1.0
	) * 1.0;

	/*displacementTexture[pos] = float4(
		h / (float(N) * float(N)),
		h / (float(N) * float(N)),
		h / (float(N) * float(N)),
		1.0
	) * 10.0;*/
}