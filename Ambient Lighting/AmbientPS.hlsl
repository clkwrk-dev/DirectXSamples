Texture2D shaderTexture;
SamplerState samplerState;

struct PixelInput
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD;
	float3 normal : NORMAL;
};

cbuffer LightBuffer
{
	float4 ambientColor;
	float4 diffuseColor;
	float3 lightDirection;
	float padding;
};

float4 main(PixelInput input) : SV_TARGET
{
	float4 textureColor = shaderTexture.Sample(samplerState, input.tex);
	float3 lightDir = -lightDirection;
	float4 diffuseFactor = max(dot(lightDir, input.normal), 0);	// Calculate how much of the original light the surface receives based on the angle between
																// the light vector and surface normal (Lambert's cosine law).

	float4 ambientLight = ambientColor * textureColor;	// Calculate the ambient color.

	float4 color = ambientLight + (diffuseFactor * diffuseColor * textureColor);

	return color;
}