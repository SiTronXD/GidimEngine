
cbuffer WaterBuffer
{
	float3 cameraPosition;
	float padding;
};

struct Input
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
	float3 worldPos : TEXCOORD1;
};

Texture2D normalMapTexture : register(t0);
SamplerState normalMapSampler : register(s0);

float calcFresnel(float3 viewDir, float3 normal)
{
	// Refractive indices
	// Air: 1.0
	// Water: 1.333
	float r0 = 0.02; // ((1.333 - 1.0) / (1.333 + 1.0)) ^ 2
	float cosTheta = saturate(dot(-viewDir, normal));

	// Fresnel Shlick's approximation
	return r0 + (1.0 - r0) * pow((1.0 - cosTheta), 5.0);
}

float4 main(Input input) : SV_TARGET
{
	float3 normal = 
		normalize(
			normalMapTexture.Sample(normalMapSampler, input.uv.xy).rgb * 2.0 - 
			1.0
		);

	// float lambert = saturate(dot(normal, normalize(-float3(1.0, -0.0, 0.0))));

	float3 viewDir = normalize(input.worldPos - cameraPosition);
	float fresnel = calcFresnel(viewDir, normal);

	return float4(float3(fresnel, fresnel, fresnel), 1.0);
}