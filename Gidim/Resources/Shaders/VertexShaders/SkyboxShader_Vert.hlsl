
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
	//float2 uv : TEXCOORD;
	float3 dir : TEXCOORD;
};

Output main(Input input)
{
	Output output;

	// Remove translation from view matrix
	matrix newViewMat = viewMatrix;
	newViewMat[3][0] = 0.0;
	newViewMat[3][1] = 0.0;
	newViewMat[3][2] = 0.0;
	newViewMat[3][3] = 1.0;

	// Position
	float4 p = float4(input.position.xyz, 1.0f);
	output.position = mul(p, worldMatrix);
	output.position = mul(output.position, newViewMat);
	output.position = mul(output.position, projectionMatrix);

	// Dir from origin
	output.dir = input.position.xyz;

	return output;
}