struct VertexInput
{
	float4 position : POSITION;
	float2 tex		: TEXCOORD;
	float3 normal	: NORMAL;
};

struct PixelInput
{
	float4 position : SV_POSITION;
	float2 tex		: TEXCOORD;
	float3 normal	: NORMAL;
};

PixelInput main( VertexInput input )
{
	PixelInput output;
	output.position = input.position;
	output.tex = input.tex;
	output.normal = normalize(input.normal);

	return output;
}