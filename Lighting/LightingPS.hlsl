Texture2D shaderTexture;
SamplerState samplerState;

struct PixelInput
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 worldPos : POSITION;
	float3 viewDirection : TEXCOORD1;
};

struct DirectionalLight
{
	float4 diffuseColor;
	float4 ambientColor;
	float4 specularColor;
	float3 direction;
	float specularPower;
};

struct PointLight
{
	float4 diffuseColor;
	float4 ambientColor;
	float4 specularColor;
	float3 position;
	float range;
	float3 attenuation;
	float specularPower;
};

struct SpotLight
{
	// TODO
};

cbuffer LightBuffer
{
	DirectionalLight directionalLight;
	PointLight pointLight;
};

void computeDirectionalLight(DirectionalLight directionalLight,
	float3 pos, float3 normal, float3 viewDirection, out float4 ambient, out float4 diffuse, out float4 specular)
{
	ambient = directionalLight.ambientColor;
	diffuse = float4(0.0f, 0.f, 0.0f, 0.0f);
	specular = float4(0.0f, 0.f, 0.0f, 0.0f);

	float3 lightVec = -directionalLight.direction;
	float diffuseFactor = dot(lightVec, normal);	// Calculate how much of the original light the surface receives based on the angle between
													// the light vector and surface normal (Lambert's cosine law).

	[flatten]
	if (diffuseFactor > 0.0f)
	{
		float3 reflectVec = reflect(-lightVec, normal);
		float specularFactor = pow(saturate(dot(reflectVec, viewDirection)), directionalLight.specularPower);
		diffuse = diffuseFactor * directionalLight.diffuseColor;
		specular = specularFactor * directionalLight.specularColor;
	}
}

void computePointLight(PointLight pointLight,
	float3 pos, float3 normal, float3 viewDirection, out float4 ambient, out float4 diffuse, out float4 specular)
{
	ambient = pointLight.ambientColor;
	diffuse = float4(0.0f, 0.f, 0.0f, 0.0f);
	specular = float4(0.0f, 0.f, 0.0f, 0.0f);

	float3 lightVec = pointLight.position - pos;
	float distance = length(lightVec);
	lightVec /= distance;

	float diffuseFactor = dot(lightVec, normal);

	[flatten]
	if (diffuseFactor > 0.0f)
	{
		float3 reflectVec = reflect(-lightVec, normal);
		float specularFactor = pow(max(dot(reflectVec, viewDirection), 0.0f), pointLight.specularPower);
		diffuse = diffuseFactor * pointLight.diffuseColor;
		specular = specularFactor * pointLight.specularColor;
	}

	float att = 1.0f / dot(pointLight.attenuation, float3(1.0f, distance, distance * distance));
	diffuse *= att;
	specular *= att;
}

float4 main(PixelInput input) : SV_TARGET
{
	float4 textureColor = shaderTexture.Sample(samplerState, input.tex);
	/*float3 lightDir = -lightDirection;
	float diffuseFactor = dot(lightDir, input.normal);*/	// Calculate how much of the original light the surface receives based on the angle between
														// the light vector and surface normal (Lambert's cosine law).
	float4 ambientLight = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 diffuseLight = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 specularLight = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float4 a, d, s;

	/*computeDirectionalLight(directionalLight, input.pos, input.normal, input.viewDirection, a, d, s);
	ambientLight += a;
	diffuseLight += d;
	specularLight += s;*/

	computePointLight(pointLight, input.worldPos, input.normal, input.viewDirection, a, d, s);
	ambientLight += a;
	diffuseLight += d;
	specularLight += s;

	return (ambientLight + diffuseLight + specularLight) * textureColor;
}