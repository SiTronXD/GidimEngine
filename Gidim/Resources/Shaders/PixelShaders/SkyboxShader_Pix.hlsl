struct Input
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

//Texture2D shaderTexture : register(t0);
//SamplerState sampleType : register(s0);

float4 main(Input input) : SV_TARGET
{
	float2 uv = input.uv;

	// Gradients
	const float3 highCol = float3(0.4, 0.1, 0.1);
	const float3 midCol = float3(0.5, 0.15, 0.15);
	const float3 lowCol = float3(0.3, 0.3, 0.5);

	float3 col = lerp(highCol, midCol, saturate(uv.y*2.0f));
	col = lerp(col, lowCol, saturate(uv.y*2.0f - 1.0f));

	// Sun
	float3 sunCol = float3(0.8, 0.3, 0.1);
	float d = saturate(length((uv - float2(0.5f, 0.5f)) * float2(2.0f, 1.0f)) * 8.0f);
	d = pow(d, 10.0);
	col = lerp(sunCol, col, d);

	return float4(col, 1.0f);
}