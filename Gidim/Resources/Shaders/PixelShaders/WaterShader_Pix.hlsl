
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

uint wangHash(inout uint seed)
{
	seed = uint(seed ^ uint(61)) ^ uint(seed >> uint(16));
	seed *= uint(9);
	seed = seed ^ (seed >> 4);
	seed *= uint(0x27d4eb2d);
	seed = seed ^ (seed >> 15);

	return seed;
}

// Generates a random number from 0 to 1
float randomFloat(inout uint state)
{
	return clamp(float(wangHash(state)) / 4294967296.0, 0.001, 1.0);
}

float calcFresnel(float3 viewDir, float3 normal)
{
	// Refractive indices
	// Air: 1.0
	// Water: 1.333
	float r0 = 0.02; // ((1.333 - 1.0) / (1.333 + 1.0)) ^ 2
	float cosTheta = saturate(dot(-viewDir, normal));

	// Optimized integer exponent
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

	// "Fake" refracted color
	float3 refractedColor = float3(0.0235, 0.1882, 0.251);
	uint randomState = uint(input.uv.x * 1024 + input.uv.y * 1024 * 1024);
	float bandingRemoval = randomFloat(randomState) * 0.01;

	// Interpolation was inspired by this shader:
	// https://www.shadertoy.com/view/Ms2SD1
	float3 dist = input.worldPos - cameraPosition;
	float distFalloff = max(1.0 - dot(dist, dist) * 0.001, 0.0);
	refractedColor = lerp(
		refractedColor,
		float3(0.8, 0.9, 0.6) * 0.6,
		(input.worldPos.y + 0.2) * 0.5 * distFalloff + bandingRemoval
	);

	// Foam
	float foamMask = foamMaskTexture.Sample(textureSampler, input.uv).r;
	float3 foamColor = foamTexture.Sample(textureSampler, input.uv).rgb;

	// Color
	float3 col = lerp(refractedColor, reflectedColor, fresnel);
	col = lerp(col, foamColor, foamMask);

	return float4(col, 1.0);
}