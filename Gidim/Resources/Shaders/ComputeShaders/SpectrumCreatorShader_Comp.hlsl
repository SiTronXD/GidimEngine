
// This ocean implementation are based on these papers:
// Fynn-Jorin Flügge. "Realtime GPGPU FFT Ocean Water Simulation". (https://tore.tuhh.de/handle/11420/1439?locale=en, accessed April 24, 2021)
// Jerry Tessendorf. "Simulating Ocean Water". (https://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.161.9102&rep=rep1&type=pdf, accessed April 28, 2021)

cbuffer CreatorBuffer : register(b0)
{
	int gridWidth;
	int gridHeight;

	float2 windDirection;

	float horizontalSize;
	float windSpeed;
	float amplitude;
	float waveDirectionTendency;
};

RWTexture2D<float4> spectrumTexture : register(u0);

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
	return clamp(float(wangHash(state)) / 4294967296.0, 0.001, 1.0);
}

// Generates 4 random and normally distributed numbers
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

[numthreads(16, 16, 1)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
	// Create initial random state
	uint randomState = uint(dispatchThreadID.x + dispatchThreadID.y * gridWidth) * uint(5243);

	float2 pos = dispatchThreadID.xy - (float2(gridWidth, gridHeight) * 0.5);
	
	// Variables required for the pillips spectrum
	float2 k = pos * 2.0 * _PI / horizontalSize;
	float L_ = windSpeed * windSpeed / _G;
	float kMag = length(k);
	float kMagSqrd = kMag * kMag;
	kMagSqrd = max(0.0001, kMagSqrd); // Avoid division by 0

	// Phillips spectrum
	// abs(dot(normalize(-k), windDir)) == abs(dot(normalize(k), windDir))
	float phillSpec = amplitude * exp(-1.0 / (kMagSqrd * L_ * L_)) /
		(kMagSqrd * kMagSqrd) *
		pow(abs(dot(normalize(k), normalize(windDirection))), waveDirectionTendency);

	// Suppress small waves
	//phillSpec *= exp(-kMagSqrd * pow(horizontalSize / 2000.0, 2.0));

	// Square root and clamp
	phillSpec = clamp(sqrt(phillSpec * 0.5), -4000.0, 4000.0);


	// Get 4 random numbers
	float4 rndGaussNums = randomBoxMuller(randomState);

	// R: real component of value 1
	// G: imaginary component of value 1
	// B: real component of value 2
	// A: imaginary component of value 2
	spectrumTexture[dispatchThreadID.xy] = rndGaussNums * phillSpec;
}