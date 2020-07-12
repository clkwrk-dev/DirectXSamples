Texture2D shaderTexture;
SamplerState samplerState;

cbuffer LightBuffer
{
	float4 diffuseColor;
	float3 lightDirection;
	float padding;
};

struct PixelInput
{
	float4 position : SV_POSITION;
	float2 tex		: TEXCOORD;
	float3 normal	: NORMAL;
};

float4 main(PixelInput input) : SV_TARGET
{
	float4 textureColor = shaderTexture.Sample(samplerState, input.tex);
	float3 lightDir = -lightDirection;
	float lightIntensity = saturate(dot(input.normal, lightDir));
	float4 color = saturate(diffuseColor * lightIntensity);
	color = color * textureColor;

	return color;
}