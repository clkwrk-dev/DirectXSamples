Texture2D shaderTexture;
SamplerState samplerState;

struct PixelInput
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD;
};

float4 main(PixelInput input) : SV_TARGET
{
	return shaderTexture.Sample(samplerState, input.tex);
}