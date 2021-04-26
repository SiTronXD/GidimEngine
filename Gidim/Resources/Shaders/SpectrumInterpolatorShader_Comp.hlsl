
cbuffer SpectrumInterpolationBufferType : register(b0)
{
	float time;
	float padding1;
	float padding2;
	float padding3;
};

RWTexture2D<float4> spectrumTexture0 : register(u0);
RWTexture2D<float4> spectrumTexture1 : register(u1);
RWTexture2D<float4> finalSpectrumTexture : register(u2);




[numthreads(16, 16, 1)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
	finalSpectrumTexture[dispatchThreadID.xy] = float4(0.0, time, 0.0, 1.0);
		//spectrumTexture0[dispatchThreadID.xy] * float4(1.0, 0.1, 0.1, 1.0);
}