
cbuffer MatrixBuffer
{
	matrix projectionMatrix;
	matrix viewMatrix;
	matrix worldMatrix;
};

struct Input
{
	float2 position : POSITION;
	float3 color : COLOR;
};

struct Output
{
	float4 position : SV_POSITION;
	float3 color : COLOR;
};

Output main(Input input)
{
	Output output;

	// Position
	float4 p = float4(input.position.xy, 0.0f, 1.0f);
	output.position = mul(p, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// Color
	output.color = input.color;

	return output;
}