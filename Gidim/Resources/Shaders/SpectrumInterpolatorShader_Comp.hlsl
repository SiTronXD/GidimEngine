
cbuffer SpectrumInterpolatorBuffer : register(b0)
{
	int gridWidth;
	int gridHeight;
	float horizontalSize;
	float time;
};

RWTexture2D<float4> spectrumTexture0 : register(u0);
RWTexture2D<float4> spectrumTexture1 : register(u1);
RWTexture2D<float4> finalSpectrumTexture0 : register(u2);


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
	// Get complex numbers from the textures
	complex h0K = createComplex(spectrumTexture0[dispatchThreadID.xy].rg);
	complex h0MinusKConj = createComplexConj(spectrumTexture1[dispatchThreadID.xy].rg);

	float2 pos = dispatchThreadID.xy - (float2(gridWidth, gridHeight) * 0.5);

	float2 k = float2(
		2.0 * _PI * pos.x / horizontalSize, 
		2.0 * _PI * pos.y / horizontalSize
	);
	float kMag = length(k);
	kMag = max(0.0001, kMag); // Avoid division by 0
	float w = sqrt(_G * kMag);
	float cosWT = cos(w * time);
	float sinWT = sin(w * time);

	complex expIWT = createComplex(cosWT, sinWT);
	complex expMinusIWT = createComplex(cosWT, -sinWT); // Negative angle
	complex hKT_Y = complexAdd(complexMul(h0K, expIWT), complexMul(h0MinusKConj, expMinusIWT));

	// Store the interpolated values twice for gradually calculating FFTs later
	finalSpectrumTexture0[dispatchThreadID.xy] = float4(
		hKT_Y.realNum, hKT_Y.imgNum, 
		hKT_Y.realNum, hKT_Y.imgNum
	);
}