
#include "lighthelper.fx"

//////////////
// TYPEDEFS //
//////////////
cbuffer cbPerFrame{
	Light	gLight;
	int		gLightType;
	float3	gEyePosW;
};

cbuffer cbPerObject{
	float4x4 worldMatrix;
	float4x4 viewMatrix;
	float4x4 projectionMatrix;

	float4x4 wvpMatrix;
};

struct VertexInputType
{
    float3 position : POSITION;
	float3 normal	: NORMAL;
    float4 color	: COLOR;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
	float3 positionW: POSITION;
	float4 normal	: NORMAL;
    float4 color	: COLOR0;
};

struct GeometryOutputType
{
	float4 posH		: SV_POSITION;
	float3 posW		: POSITION;
	float3 normalW	: NORMAL;
	float4 color	: COLOR0;
	uint primID		: SV_PrimitiveID;
};



////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType ColorVertexShader(VertexInputType input)
{
	PixelInputType output;
    // Calculate the position of the vertex against the world, view, and projection matrices
	output.position = mul(float4(input.position, 1.0f), wvpMatrix);
    output.normal	= mul(float4(input.normal, 0.0f), worldMatrix);

	// Store the texture coordinates for the pixel shader.
	output.positionW = mul(float4(input.position, 1.0f), wvpMatrix);
    // Store the input color for the pixel shader to use.
    output.color = input.color;
    
    return output;
}

////////////////////////////////////////////////////////////////////////////////
// Geometry Shader
////////////////////////////////////////////////////////////////////////////////
[maxvertexcount(3)]
void ColorGeometryShader(point PixelInputType gIn[1],uint primID : SV_PrimitiveID,inout TriangleStream<GeometryOutputType> triStream)
{
	GeometryOutputType output;
	for (int i = 0; i < 1; i++){
		output.posH = gIn[i].position;
		output.posW = gIn[i].positionW;
		output.normalW = gIn[i].normal;
		output.color = gIn[i].color;
	}
	triStream.Append(output);
}

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 ColorPixelShader(GeometryOutputType input) : SV_Target
{
	// Interpolating normal can make it not be of unit length so normalize it.
    float3 normalW = normalize(input.normalW);

	// Compute the lit color for this pixel.
	SurfaceInfo v = {input.posW, normalW, input.color, input.color};
	float3 litColor = ParallelLight(v, gLight, gEyePosW);

	return float4(litColor, 1.0f);	
}

technique10 ExplodeTechnique
{
    pass pass0
    {
        SetVertexShader(CompileShader(vs_4_0, ColorVertexShader()));
		SetGeometryShader(CompileShader(vs_4_0, ColorGeometryShader()));
        SetPixelShader(CompileShader(ps_4_0, ColorPixelShader()));
       		
    }
}
