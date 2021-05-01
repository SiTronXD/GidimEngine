
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

Output main(Input input)
{
	Output output;

	// Position
	float4 p = float4(input.position.xyz, 1.0f);

	//p.y += sin(p.x * 3243.43 + p.z * 5532.123) * 0.7;

	output.position = mul(p, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// UV coordinates
	output.uv = input.uv;

	return output;
}