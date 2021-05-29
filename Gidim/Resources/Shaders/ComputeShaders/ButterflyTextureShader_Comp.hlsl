
cbuffer ButterflyTextureBuffer : register(b0)
{
	int gridSize;
	int padding[3];
};

RWTexture2D<float4> finalButterflyTexture : register(u0);


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

// Preprocessor directives for creating an array with reversed bits at each index
/*#define R2(n) n, n + 2*64, n + 1*64, n + 3*64
#define R4(n) R2(n), R2(n + 2*16), R2(n + 1*16), R2(n + 3*16)
#define R6(n) R4(n), R4(n + 2*4 ), R4(n + 1*4 ), R4(n + 3*4 )
#define REVERSE_BITS R6(0), R6(2), R6(1), R6(3)*/

// Reverse the bits
uint bitReversed(int input)
{
	// Fast and simple lookup table
	/*uint lookup[256] = { REVERSE_BITS };

	return lookup[input];*/


	// Move each individual bit
	uint numBits = log2(gridSize);
	uint num = uint(input);
	uint createdNum = 0;

	for (uint i = 0; i < numBits; ++i)
	{
		// Get current bit
		uint mask = 1 << i;
		uint maskResult = mask & num;

		// Calculate amount to move
		int moveAmount = ((numBits - 2 * i) - 1);

		// Don't move in a negative direction
		if (moveAmount > 0)
			maskResult = maskResult << moveAmount;
		else
			maskResult = maskResult >> -moveAmount;

		// Combine the new bit
		createdNum = createdNum | maskResult;
	}

	return createdNum;
}

[numthreads(2, 16, 1)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
	float2 pos = dispatchThreadID.xy;
	float k = (pos.y * float(gridSize) / pow(2.0, pos.x + 1)) % gridSize;
	complex twiddleFactor = createComplex(
		cos(2.0 * _PI * k / float(gridSize)),
		sin(2.0 * _PI * k / float(gridSize))
	);

	int butterflySpan = int(pow(2.0, pos.x));
	int butterflyWing = 0;

	if (pos.y % pow(2, pos.x + 1) < pow(2, pos.x))
		butterflyWing = 1;

	// Only the first stage requires bit reversal
	if (pos.x == 0)
	{
		finalButterflyTexture[dispatchThreadID.xy] = float4(
			twiddleFactor.realNum,
			twiddleFactor.imgNum,
			bitReversed(pos.y - (butterflySpan * (1 - butterflyWing))),
			bitReversed(pos.y + (butterflySpan * butterflyWing))
		);
	}
	else
	{
		finalButterflyTexture[dispatchThreadID.xy] = float4(
			twiddleFactor.realNum,
			twiddleFactor.imgNum,
			pos.y - (butterflySpan * (1 - butterflyWing)),
			pos.y + (butterflySpan * butterflyWing)
		);
	}
}