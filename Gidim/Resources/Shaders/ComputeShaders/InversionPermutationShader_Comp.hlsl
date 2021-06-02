
cbuffer InvPermBuffer : register(b0)
{
	int gridSize;
	int pingPong;
	int padding[2];
};

RWTexture2D<float4> spectrumTextureX : register(u0);
RWTexture2D<float4> spectrumTextureY : register(u1);
RWTexture2D<float4> spectrumTextureZ : register(u2);
RWTexture2D<float4> displacementTexture : register(u3);


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
	uint2 pos = dispatchThreadID.xy;

	// Get permutation for this element
	float perms[] = { 1.0, -1.0 };
	uint index = uint(pos.x + pos.y) % 2;
	float curPerm = perms[index];

	float componentX = 0.0;
	float componentY = 0.0;
	float componentZ = 0.0;

	// Get the real component from correct channel
	if (pingPong == 0)
	{
		componentX = spectrumTextureX[pos].r;
		componentY = spectrumTextureY[pos].r;
		componentZ = spectrumTextureZ[pos].r;
	}
	else if (pingPong == 1)
	{
		componentX = spectrumTextureX[pos].b;
		componentY = spectrumTextureY[pos].b;
		componentZ = spectrumTextureZ[pos].b;
	}

	float invPermScalar = curPerm / (float(gridSize) * float(gridSize));

	displacementTexture[pos] = float4(
		componentX,
		componentY,
		componentZ,
		0.0
	) * invPermScalar;
}