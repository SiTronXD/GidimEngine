
/*cbuffer FoamMaskBuffer : register(b0)
{
	
};*/

RWTexture2D<float4> displacementTexture : register(u0);
RWTexture2D<float4> foamMaskTexture : register(u1);

[numthreads(16, 16, 1)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
	
}