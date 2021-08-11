
struct Input
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
	float3 color : TEXCOORD1;
};

float4 main(Input input) : SV_TARGET
{
	return float4(input.color, 1.0f);
}