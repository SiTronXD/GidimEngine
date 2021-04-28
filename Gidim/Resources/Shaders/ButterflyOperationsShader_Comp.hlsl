
cbuffer ButterflyOperationsBuffer : register(b0)
{
	int stage;
	int pingPong;
	int direction;
	int padding1;
};

RWTexture2D<float4> butterflyTexture : register(u0);
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

void horizontalButterfly(uint2 pos)
{
	float4 data = butterflyTexture[uint2(stage, pos.x)];

	if (pingPong == 0)
	{
		float2 _p = pingPong0[uint2(data.z, pos.y)].rg;
		float2 _q = pingPong0[uint2(data.w, pos.y)].rg;
		float2 _w = data.xy;

		complex compP = createComplex(_p);
		complex compQ = createComplex(_q);
		complex compW = createComplex(_w);

		complex h = complexAdd(compP, complexMul(compW, compQ));

		pingPong1[pos] = float4(h.realNum, h.imgNum, 0.0, 1.0);
	}
	else if (pingPong == 1)
	{
		float2 _p = pingPong1[uint2(data.z, pos.y)].rg;
		float2 _q = pingPong1[uint2(data.w, pos.y)].rg;
		float2 _w = data.xy;

		complex compP = createComplex(_p);
		complex compQ = createComplex(_q);
		complex compW = createComplex(_w);

		complex h = complexAdd(compP, complexMul(compW, compQ));

		pingPong0[pos] = float4(h.realNum, h.imgNum, 0.0, 1.0);
	}
}

void verticalButterfly(uint2 pos)
{
	float4 data = butterflyTexture[uint2(stage, pos.y)];

	if (pingPong == 0)
	{
		float2 _p = pingPong0[uint2(pos.x, data.z)].rg;
		float2 _q = pingPong0[uint2(pos.x, data.w)].rg;
		float2 _w = data.xy;

		complex compP = createComplex(_p);
		complex compQ = createComplex(_q);
		complex compW = createComplex(_w);

		complex h = complexAdd(compP, complexMul(compW, compQ));

		pingPong1[pos] = float4(h.realNum, h.imgNum, 0.0, 1.0);
	}
	else if (pingPong == 1)
	{
		float2 _p = pingPong1[uint2(pos.x, data.z)].rg;
		float2 _q = pingPong1[uint2(pos.x, data.w)].rg;
		float2 _w = data.xy;

		complex compP = createComplex(_p);
		complex compQ = createComplex(_q);
		complex compW = createComplex(_w);

		complex h = complexAdd(compP, complexMul(compW, compQ));

		pingPong0[pos] = float4(h.realNum, h.imgNum, 0.0, 1.0);
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