
struct Input
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

float4 main(Input input) : SV_TARGET
{
	return float4(float3(input.uv, 0.0), 1.0);
}