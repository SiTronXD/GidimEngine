
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

StructuredBuffer<float4x4> boidsBuffer : register(t0);

Output main(Input input)
{
	Output output;

	// Position
	float4 p = float4(input.position.xyz, 1.0f);
	output.position = mul(p, boidsBuffer[id]);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// UV coordinates
	output.uv = input.uv;

	// Color
	output.color = vertexColor;

	return output;
}