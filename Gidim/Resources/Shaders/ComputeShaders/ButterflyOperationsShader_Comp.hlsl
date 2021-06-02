
cbuffer ButterflyOperationsBuffer : register(b0)
{
	int stage;
	int pingPong;
	int direction;
	int padding;
};

RWTexture2D<float4> butterflyTexture : register(u0);
RWTexture2D<float4> spectrumTexture : register(u1);


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

complex butterflyOperation(float2 _p, float2 _q, float2 _w)
{
	complex compP = createComplex(_p);
	complex compQ = createComplex(_q);
	complex compW = createComplex(_w);

	return complexAdd(compP, complexMul(compW, compQ));
}

void horizontalButterfly(uint2 threadPos)
{
	float4 data = butterflyTexture[uint2(stage, threadPos.x)];
	float2 _w = data.xy;
	float4 _p = spectrumTexture[uint2(data.z, threadPos.y)];
	float4 _q = spectrumTexture[uint2(data.w, threadPos.y)];

	// Read RG, write BA
	if (pingPong == 0)
	{
		complex h = butterflyOperation(_p.rg, _q.rg, _w);

		spectrumTexture[threadPos] = float4(spectrumTexture[threadPos].rg, h.realNum, h.imgNum);
	}
	// Read BA, write RG
	else if (pingPong == 1)
	{
		complex h = butterflyOperation(_p.ba, _q.ba, _w);

		spectrumTexture[threadPos] = float4(h.realNum, h.imgNum, spectrumTexture[threadPos].ba);
	}
}

void verticalButterfly(uint2 threadPos)
{
	float4 data = butterflyTexture[uint2(stage, threadPos.y)];
	float2 _w = data.xy;
	float4 _p = spectrumTexture[uint2(threadPos.x, data.z)];
	float4 _q = spectrumTexture[uint2(threadPos.x, data.w)];

	// Read RG, write BA
	if (pingPong == 0)
	{
		complex h = butterflyOperation(_p.rg, _q.rg, _w);

		spectrumTexture[threadPos] = float4(spectrumTexture[threadPos].rg, h.realNum, h.imgNum);
	}
	// Read BA, write RG
	else if (pingPong == 1)
	{
		complex h = butterflyOperation(_p.ba, _q.ba, _w);

		spectrumTexture[threadPos] = float4(h.realNum, h.imgNum, spectrumTexture[threadPos].ba);
	}
}


[numthreads(16, 16, 1)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
	if (direction == 0)
		horizontalButterfly(dispatchThreadID.xy);
	else if(direction == 1)
		verticalButterfly(dispatchThreadID.xy);
}