
cbuffer MatrixBuffer : register(b0)
{
	matrix projectionMatrix;
	matrix viewMatrix;
	matrix worldMatrix;
};

cbuffer BoidBuffer : register(b1)
{
	int id;
	float3 vertexColor;
}

struct Input
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
};

struct Output
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
	float3 color : TEXCOORD1;
};

StructuredBuffer<float3> boidsBuffer : register(t0);

Output main(Input input)
{
	Output output;

	float3 offsetPos = boidsBuffer[id].xyz;

	// Position
	float4 p = float4(input.position.xyz, 1.0f);
	p += float4(offsetPos, 0.0f);
	output.position = mul(p, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// UV coordinates
	output.uv = input.uv;

	// Color
	output.color = vertexColor;

	return output;
}