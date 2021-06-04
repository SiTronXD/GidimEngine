
cbuffer MatrixBuffer : register(b0)
{
	matrix projectionMatrix;
	matrix viewMatrix;
	matrix worldMatrix;
};

cbuffer WaterVertexBuffer : register(b1)
{
	float lambdaDispScale;

	float padding[3];
}

struct Input
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
};

struct Output
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD0;
	float3 worldPos : TEXCOORD1;
};

Texture2D displacementTexture : register(t0);
SamplerState displacementSampler;

Output main(Input input)
{
	Output output;

	// Displacement
	float3 displacementPos = displacementTexture.SampleLevel(
		displacementSampler,
		input.uv,
		0
	).rgb * lambdaDispScale;

	// Position
	float4 p = float4(input.position.xyz, 1.0);
	output.position = mul(p, worldMatrix);
	output.position.xyz += displacementPos;	// World space displacement
	output.worldPos = output.position.xyz;

	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// UV coordinates
	output.uv = input.uv;

	return output;
}