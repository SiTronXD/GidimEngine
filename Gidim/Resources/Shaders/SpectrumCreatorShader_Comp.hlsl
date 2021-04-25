
RWTexture2D<float4> spectrumTexture0 : register(u0);
RWTexture2D<float4> spectrumTexture1 : register(u1);

#define _PI 3.14159265
#define _G 9.80665


uint wangHash(inout uint seed)
{
	seed = uint(seed ^ uint(61)) ^ uint(seed >> uint(16));
	seed *= uint(9);
	seed = seed ^ (seed >> 4);
	seed *= uint(0x27d4eb2d);
	seed = seed ^ (seed >> 15);

	return seed;
}

// Generates a random number from 0 to 1
float randomFloat(inout uint state)
{
	return float(wangHash(state)) / 4294967296.0;
}

// Generates 4 random, normally distributed numbers
float4 randomBoxMuller(inout uint randomState)
{
	float theta0 = 2.0 * _PI * randomFloat(randomState);
	float theta1 = 2.0 * _PI * randomFloat(randomState);

	float radius0 = sqrt(-2.0 * log(randomFloat(randomState)));
	float radius1 = sqrt(-2.0 * log(randomFloat(randomState)));

	float4 randomNums = float4(
		radius0 * cos(theta0),
		radius0 * sin(theta0),
		radius1 * cos(theta1),
		radius1 * sin(theta1)
	);

	return randomNums;
}

float phillipsSpectrum(float2 k, float horizontalSize, 
	float L_, float amplitude, float2 windDirection)
{
	float kMag = length(k);
	kMag = max(0.0001, kMag);

	float num = amplitude * exp(-1.0 / (kMag * kMag * L_ * L_)) / 
		(kMag * kMag * kMag * kMag) * 
		pow(abs(dot(normalize(k), normalize(windDirection))), 6.0);

	// Suppress small waves
	num *= exp(-kMag * kMag * pow(horizontalSize / 2000.0, 2));

	return clamp(num, -4000.0, 4000.0);
}

[numthreads(16, 16, 1)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
	int gridWidth = 256;
	int gridHeight = 256;

	float horizontalSize = 1000.0;
	float windSpeed = 26.0;
	float amplitude = 20.0;
	float2 windDirection = float2(1.0, 0.0);

	// Create initial random state
	uint randomState = uint(dispatchThreadID.x + dispatchThreadID.y * gridWidth) * uint(5243);

	float2 pos = dispatchThreadID.xy;// -(float2(gridWidth, gridHeight) * 0.5);
	//float2 pos = dispatchThreadID.xy -(float2(gridWidth, gridHeight) * 0.5);
	
	float2 k = float2(
		2.0 * _PI * pos.x / horizontalSize,
		2.0 * _PI * pos.y / horizontalSize
	);

	float L_ = windSpeed * windSpeed / _G;

	float oneOverSqrtTwo = 1.0 / sqrt(2);
	float h0K = sqrt(phillipsSpectrum(k, horizontalSize, L_, amplitude, windDirection)) * oneOverSqrtTwo;
	float h0MinusK = sqrt(phillipsSpectrum(-k, horizontalSize, L_, amplitude, windDirection)) * oneOverSqrtTwo;

	float4 rndGaussNums = randomBoxMuller(randomState);

	// R: real component
	// G: imaginary component
	spectrumTexture0[dispatchThreadID.xy] = 
		float4(rndGaussNums.xy * h0K, 0.0, 1.0);

	spectrumTexture1[dispatchThreadID.xy] =
		float4(rndGaussNums.zw * h0MinusK, 0.0, 1.0);
}