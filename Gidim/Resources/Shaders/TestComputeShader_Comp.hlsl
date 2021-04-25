
RWTexture2D<float4> bufferOut : register(u0);
RWTexture2D<float4> bufferOut2 : register(u1);

[numthreads(16, 16, 1)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
	bufferOut[dispatchThreadID.xy] = float4(dispatchThreadID.xy / 256.0, 1.0, 1.0);
}