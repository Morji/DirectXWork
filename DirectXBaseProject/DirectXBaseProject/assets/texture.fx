//=============================================================================
// texture.fx
//
// Textures geometry - handles regular and multitexturing
//=============================================================================

#include "lighthelper.fx"

cbuffer cbPerFrame{
	Light	gLight;
	float3	gEyePosW;
	int		texType;	//handle different types of texturing: 0 - regular; 1 - multitexturing
};

bool gSpecularEnabled;

cbuffer cbPerObject{
	float4x4	worldMatrix;
	float4x4	viewMatrix;
	float4x4	projectionMatrix;
	
};
// Nonnumeric values cannot be added to a cbuffer.
Texture2D	gDiffuseMap;//for regular texturing
Texture2D	gSpecMap;//for regular and multi texturing
Texture2D	gBlendMap;//for multi texturing
Texture2D	gLayer1;
Texture2D	gLayer2;
Texture2D	gLayer3;

///////////////////
// SAMPLE STATES //
///////////////////
SamplerState SampleTypeMirror{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Mirror;
    AddressV = Mirror;
};

SamplerState SampleTypeWrap{
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
	float  shade		: SHADE;
	float3 positionW	: POSITION;
	float4 normal		: NORMAL;
    float2 tex			: TEXCOORD0;//for regular texturing
	float2 tiledUV      : TEXCOORD1;//for multi texturing
    float2 stretchedUV  : TEXCOORD2;//for multi texturing 
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType TextureVertexShader(VertexInputType input){
    PixelInputType output;       

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(float4(input.position, 1.0f), worldMatrix);
	output.normal	= mul(float4(input.normal, 0.0f), worldMatrix);		

    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

	output.positionW = mul(float4(input.position, 1.0f), worldMatrix);

	//if multitexturing
	if (texType == 1){
		output.shade = saturate(max(dot(output.normal, gEyePosW), 0.0f) );
		output.tiledUV     = 16*input.tex;
		output.stretchedUV = input.tex;
	}
	else{
		// Store the texture coordinates for the pixel shader.
		output.tex = input.tex;
    }
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

	///MULTITEXTURING
	if (texType == 1){
		spec = gSpecMap.Sample( SampleTypeWrap, input.stretchedUV );
			
		// Map [0,1] --> [0,256]
		spec.a *= 256.0f;

		// Get materials from texture maps for diffuse col.		
		float4 c1 = gLayer1.Sample( SampleTypeWrap, input.stretchedUV );
		float4 c2 = gLayer2.Sample( SampleTypeWrap, input.stretchedUV );
		float4 c3 = gLayer3.Sample( SampleTypeWrap, input.stretchedUV );
	
		float4 t = gBlendMap.Sample( SampleTypeWrap, input.stretchedUV ); 
	
		// Find the inverse of all the blend weights so that we can  scale the total color to the range [0, 1].
		float totalInverse = 1.0f / (t.r + t.g + t.b);
    
		// Scale the colors by each layer by its corresponding weight
		// stored in the blendmap.  
		c1 *= t.r * totalInverse;
		c2 *= t.g * totalInverse;
		c3 *= t.b * totalInverse;

		//shade*diffuse component
		return (input.shade*(c1+c2+c3));
	}
	else{
		spec = gSpecMap.Sample( SampleTypeMirror, input.tex );
		// Map [0,1] --> [0,256]
		spec.a *= 256.0f;

		// Get materials from texture maps.
		float4 diffuse = gDiffuseMap.Sample( SampleTypeMirror, input.tex );	
    
		// Compute the lit color for this pixel.
		SurfaceInfo v = {input.positionW, normalW, diffuse, spec};
		float3 litColor = ParallelLight(v, gLight, gEyePosW);
    
		return float4(litColor, diffuse.a);	
	}
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