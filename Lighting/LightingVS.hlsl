cbuffer ConstantBuffer
{
	float4x4 worldMatrix;
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
};

cbuffer CameraBuffer
{
	float3 cameraPosition;
	float1 padding;
};

struct VertexInput
{
	float4 pos : POSITION;
	float2 tex : TEXCOORD;
	float3 normal : NORMAL;
};

struct PixelInput
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 viewDirection : TEXCOORD1;
};

PixelInput main(VertexInput input)
{
	PixelInput output;

	input.pos.w = 1.0f;

	output.pos = mul(input.pos, worldMatrix);
	output.pos = mul(output.pos, viewMatrix);
	output.pos = mul(output.pos, projectionMatrix);

	output.tex = input.tex;

	output.normal = mul(input.normal, (float3x3)worldMatrix);
	output.normal = normalize(output.normal);

	float3 worldPosition = mul(input.pos, worldMatrix).xyz;
	output.viewDirection = normalize(cameraPosition - worldPosition);

	return output;
}