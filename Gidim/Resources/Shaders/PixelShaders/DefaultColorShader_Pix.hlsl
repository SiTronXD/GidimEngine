struct Input
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

float4 main(Input input) : SV_TARGET
{
	float3 col = float3(0.3, 0.3, 0.8);

	return float4(col, 1.0f);
}