
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
TextureCube skyboxTexture : register(t1);
SamplerState textureSampler : register(s0);

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
	// Get normal from normal map
	float3 normal = 
		normalize(
			normalMapTexture.Sample(textureSampler, input.uv.xy).rgb * 2.0 -
			1.0
		);

	// View direction and fresnel
	float3 viewDir = normalize(input.worldPos - cameraPosition);
	float fresnel = calcFresnel(viewDir, normal);

	// Reflected skybox color
	float3 reflectedColor = skyboxTexture.Sample(
		textureSampler, 
		reflect(viewDir, normal)
	);

	// Color
	float3 col = reflectedColor; // lerp(float3(0.0, 0.0, 0.0), reflectedColor, fresnel);

	return float4(col, 1.0);
}