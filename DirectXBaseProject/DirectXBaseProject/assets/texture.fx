//=============================================================================
// texture.fx
//
// Textures geometry - handles regular and multitexturing
//=============================================================================

#include "lighthelper.fx"

cbuffer cbPerFrame{
	Light	gLight;
	float3	gEyePosW;
};

cbuffer cbPerObject{
	float4x4	worldMatrix;
	float4x4	viewMatrix;
	float4x4	projectionMatrix;
	float4x4	wvpMatrix;
	
};
// Nonnumeric values cannot be added to a cbuffer.
Texture2D	gDiffuseMap;//for regular texturing
Texture2D	gSpecMap;//for regular and multi texturing

///////////////////
// SAMPLE STATES //
///////////////////
SamplerState SampleType{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

//////////////
// TYPEDEFS //
//////////////
struct VertexInputType{
    float3 position : POSITION;
	float3 normal	: NORMAL;
    float2 tex		: TEXCOORD;
};

struct PixelInputType{
    float4 position		: SV_POSITION;
	float3 positionW	: POSITION;
	float4 normal		: NORMAL;
    float2 tex			: TEXCOORD0;//for regular texturing
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType TextureVertexShader(VertexInputType input){
    PixelInputType output;       

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(float4(input.position, 1.0f), wvpMatrix);
	output.normal	= mul(float4(input.normal, 0.0f), worldMatrix);

	// Store the texture coordinates for the pixel shader.
	output.positionW = mul(float4(input.position, 1.0f), wvpMatrix);
	output.tex = input.tex;
    
    return output;
}

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 TexturePixelShader(PixelInputType input) : SV_Target
{
	// Interpolating normal can make it not be of unit length so normalize it.
    float3 normalW = normalize(input.normal);

	float4 spec;

	spec = gSpecMap.Sample( SampleType, input.tex );
	// Map [0,1] --> [0,256]
	spec.a *= 256.0f;

	// Get materials from texture maps.
	float4 diffuse = gDiffuseMap.Sample( SampleType, input.tex );	
    
	// Compute the lit color for this pixel.
	SurfaceInfo v = {input.positionW, normalW, diffuse, spec};
	float3 litColor = ParallelLight(v, gLight, gEyePosW);

	return float4(litColor, diffuse.a);	
}

////////////////////////////////////////////////////////////////////////////////
// Technique
////////////////////////////////////////////////////////////////////////////////
technique10 TextureTechnique
{
    pass pass0
    {
        SetVertexShader(CompileShader(vs_4_0, TextureVertexShader()));
		SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, TexturePixelShader()));
        
    }
}