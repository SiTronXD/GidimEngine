
struct Input
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

Texture2D shaderTexture;
SamplerState sampleType;

float4 main(Input input) : SV_TARGET
{
	float4 textureCol = shaderTexture.Sample(sampleType, input.uv.xy);

	return float4(textureCol.rgb, 1.0);
}