Texture2D shaderTexture;
SamplerState samplerState;

struct PixelInput
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 viewDirection : TEXCOORD1;
};

cbuffer LightBuffer
{
	float4 ambientColor;
	float4 diffuseColor;
	float4 specularColor;
	float3 lightDirection;
	float1 specularPower;
};

float4 main(PixelInput input) : SV_TARGET
{
	float4 textureColor = shaderTexture.Sample(samplerState, input.tex);
	float3 lightDir = -lightDirection;
	float diffuseFactor = dot(lightDir, input.normal);	// Calculate how much of the original light the surface receives based on the angle between
														// the light vector and surface normal (Lambert's cosine law).
	float4 ambientLight = ambientColor;
	float4 diffuseLight = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 specularLight = float4(0.0f, 0.0f, 0.0f, 0.0f);

	[flatten]
	if (diffuseFactor > 0.0f)
	{
		float3 reflectVec = reflect(-lightDir, input.normal);
		float specularFactor = pow(saturate(dot(reflectVec, input.viewDirection)), specularPower);
		diffuseLight = diffuseFactor * diffuseColor;
		specularLight = specularFactor * specularColor;
	}

	return (ambientLight + diffuseLight + specularLight) * textureColor;
}