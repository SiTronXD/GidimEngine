
RWTexture2D<float4> pingPongTexture0 : register(u0);
RWTexture2D<float4> pingPongTexture1 : register(u1);


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


[numthreads(16, 16, 1)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
	pingPongTexture0[dispatchThreadID.xy] = float4(
		max(sin(dispatchThreadID.x * 0.5), 0.0),
		0.0,
		0.0,
		1.0
	);
}