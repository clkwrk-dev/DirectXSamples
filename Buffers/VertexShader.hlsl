struct VertexInput
{
	float4 position : POSITION;
	float4 color : COLOR;
};

struct PixelOutput
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

PixelOutput main( VertexInput input )
{
	input.position.w = 1.0f;

	PixelOutput output;
	output.position = input.position;
	output.color = input.color;

	return output;
}