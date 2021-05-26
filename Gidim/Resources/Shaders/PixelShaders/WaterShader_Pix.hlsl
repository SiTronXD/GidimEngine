
struct Input
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

Texture2D normalMapTexture : register(t0);
SamplerState normalMapSampler : register(s0);

float4 main(Input input) : SV_TARGET
{
	float3 normal = 
		normalize(
			normalMapTexture.Sample(normalMapSampler, input.uv.xy).rgb * 2.0 - 
			float3(1.0, 1.0, 1.0)
		);

	float lambert = saturate(dot(normal, normalize(-float3(1.0, -0.0, 0.0))));

	return float4(normal, 1.0);
}