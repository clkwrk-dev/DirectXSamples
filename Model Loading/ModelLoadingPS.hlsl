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
	float3 lightDirection;
	float4 diffuseColor;
	float padding;
};

float4 main(PixelInput input) : SV_TARGET
{
	float4 textureColor = shaderTexture.Sample(samplerState, input.tex);
	float3 lightDir = -lightDirection;
	float4 diffuseFactor = max(dot(lightDir, input.normal), 0);	// Calculate how much of the original light the surface receives based on the angle between
																// the light vector and surface normal (Lambert's cosine law).

	float4 color = diffuseFactor * diffuseColor * textureColor; // Calculate the amount of light reflected based on the diffuse factor.

	return color;
}