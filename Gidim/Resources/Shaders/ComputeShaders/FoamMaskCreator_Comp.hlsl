
cbuffer FoamMaskBuffer : register(b0)
{
	int gridWidth;
	int gridHeight;

	int padding[2];
};

RWTexture2D<float4> displacementTexture : register(u0);
RWTexture2D<float4> foamMaskTexture : register(u1);

[numthreads(16, 16, 1)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
	foamMaskTexture[dispatchThreadID.xy] = float4(dispatchThreadID.xy / float2(gridWidth, gridHeight), 1.0, 1.0);
}