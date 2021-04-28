
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

uint bitReversed(uint i)
{
	return reversebits(i);
}

[numthreads(2, 16, 1)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
	int N = 256;

	float2 pos = dispatchThreadID.xy;
	float k = (pos.y * float(N) / pow(2.0, pos.x + 1)) % N;
	complex twiddleFactor = createComplex(
		cos(2.0 * _PI * k / float(N)),
		sin(2.0 * _PI * k / float(N))
	);

	int butterflySpan = int(pow(2.0, pos.x));
	int butterflyWing = 0;

	if (pos.y % pow(2, pos.x + 1) < pow(2, pos.x))
		butterflyWing = 1;

	
	finalButterflyTexture[dispatchThreadID.xy] = float4(
		twiddleFactor.realNum,
		twiddleFactor.imgNum,
		bitReversed(pos.y - (butterflySpan * (1 - butterflyWing))),
		bitReversed(pos.y + (butterflySpan * butterflyWing))
	);
}