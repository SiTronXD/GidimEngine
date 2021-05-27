
RWTexture2DArray<float4> destTexture : register(u0);

[numthreads(16, 16, 1)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
	destTexture[uint3(dispatchThreadID.xy, 3)] = 
		float4(
			dispatchThreadID.x / 256.0,
			dispatchThreadID.y / 256.0,
			0.0,
			1.0
		);
}