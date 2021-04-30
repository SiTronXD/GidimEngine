
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

Texture2D displacementTextureX : register(t0);
Texture2D displacementTextureY : register(t1);
Texture2D displacementTextureZ : register(t2);
SamplerState displacementSampler;

Output main(Input input)
{
	Output output;

	// Displacement
	float displacementColX = displacementTextureX.SampleLevel(
		displacementSampler,
		input.uv,
		0
	).r;
	float displacementColY = displacementTextureY.SampleLevel(
		displacementSampler,
		input.uv,
		0
	).r;
	float displacementColZ = displacementTextureZ.SampleLevel(
		displacementSampler,
		input.uv,
		0
	).r;

	float3 displacementPos = float3(displacementColX, displacementColY, displacementColZ) * 0.15;

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