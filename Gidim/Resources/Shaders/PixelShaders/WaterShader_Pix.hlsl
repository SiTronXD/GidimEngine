
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
Texture2D foamMaskTexture : register(t1);
Texture2D foamTexture : register(t2);
TextureCube skyboxTexture : register(t3);
SamplerState textureSampler : register(s0);

float calcFresnel(float3 viewDir, float3 normal)
{
	// Refractive indices
	// Air: 1.0
	// Water: 1.333
	float r0 = 0.02; // ((1.333 - 1.0) / (1.333 + 1.0)) ^ 2
	float cosTheta = saturate(dot(-viewDir, normal));

	// Avoid pow() with floats
	float invTheta = (1.0 - cosTheta);

	// Fresnel Shlick's approximation
	return r0 + (1.0 - r0) * invTheta * invTheta * invTheta * invTheta * invTheta;
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

	// Refracted color
	float3 refractedColor = float3(6.0, 48.0, 64.0) / 255.0;

	// Foam
	float foamMask = foamMaskTexture.Sample(textureSampler, input.uv).r;
	float3 foamColor = foamTexture.Sample(textureSampler, input.uv).rgb;

	// Color
	float3 col = lerp(refractedColor, reflectedColor, fresnel);
	col = lerp(col, foamColor, foamMask);

	return float4(col, 1.0);
}