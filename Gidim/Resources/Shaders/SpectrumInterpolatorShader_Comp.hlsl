
cbuffer SpectrumInterpolatorBuffer : register(b0)
{
	float time;
	float padding1;
	float padding2;
	float padding3;
};

RWTexture2D<float4> spectrumTexture0 : register(u0);
RWTexture2D<float4> spectrumTexture1 : register(u1);
RWTexture2D<float4> finalSpectrumTexture0 : register(u2);
RWTexture2D<float4> finalSpectrumTexture1 : register(u3);


#define _PI 3.14159265
#define _G 9.80665

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

complex createComplexConj(float2 values)
{
	return createComplex(values.x, -values.y);
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
	float horizontalSize = 1000.0;
	int gridWidth = 256;
	int gridHeight = 256;

	complex h0K = createComplex(spectrumTexture0[dispatchThreadID.xy].rg);
	complex h0MinusKConj = createComplexConj(spectrumTexture1[dispatchThreadID.xy].rg);

	float2 pos = dispatchThreadID.xy - (float2(gridWidth, gridHeight) * 0.5);
	//float2 pos = dispatchThreadID.xy;

	float2 k = float2(
		2.0 * _PI * pos.x / horizontalSize, 
		2.0 * _PI * pos.y / horizontalSize
	);
	float kMag = length(k);
	kMag = max(0.0001, kMag);

	float w = sqrt(_G * kMag);

	float cosWT = cos(w * time);
	float sinWT = sin(w * time);

	complex expIWT = createComplex(cosWT, sinWT);
	complex expMinusIWT = createComplex(cosWT, -sinWT);

	complex hKT_Y = complexAdd(complexMul(h0K, expIWT), complexMul(h0MinusKConj, expMinusIWT));

	finalSpectrumTexture0[dispatchThreadID.xy] = float4(hKT_Y.realNum, hKT_Y.imgNum, 0.0, 1.0);
	finalSpectrumTexture1[dispatchThreadID.xy] = float4(hKT_Y.realNum, hKT_Y.imgNum, 0.0, 1.0);
}