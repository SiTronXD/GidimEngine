
cbuffer MatrixBuffer
{
	matrix projectionMatrix;
	matrix viewMatrix;
	matrix worldMatrix;
};

struct Input
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
};

struct Output
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

Texture2D displacementTexture : register(t0);
SamplerState displacementSampler;

Output main(Input input)
{
	Output output;

	// Displacement
	float displacementCol = displacementTexture.SampleLevel(
		displacementSampler,
		input.uv,
		0
	).r;

	float3 displacementPos = float3(0.0, displacementCol, 0.0) * 0.15;

	// Position
	float4 p = float4(input.position.xyz, 1.0);
	output.position = mul(p, worldMatrix);
	output.position.xyz += displacementPos;	// World space displacement
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// UV coordinates
	output.uv = input.uv;

	return output;
}