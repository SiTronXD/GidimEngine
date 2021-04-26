
RWTexture2D<float4> spectrumTexture0 : register(u0);
RWTexture2D<float4> spectrumTexture1 : register(u1);
RWTexture2D<float4> finalSpectrumTexture : register(u2);


[numthreads(16, 16, 1)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
	finalSpectrumTexture[dispatchThreadID.xy] =
		spectrumTexture0[dispatchThreadID.xy] * float4(1.0, 0.1, 0.1, 1.0);
}