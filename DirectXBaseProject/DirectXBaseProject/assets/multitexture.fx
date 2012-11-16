//=============================================================================
// texture.fx
//
// Textures geometry - handles regular and multitexturing
//=============================================================================

#include "lighthelper.fx"

cbuffer cbPerFrame{
	Light	gLight;
	int		gLightType;
	float3	gEyePosW;
};

bool gSpecularEnabled;

cbuffer cbPerObject{
	float4x4	worldMatrix;
	float4x4	viewMatrix;
	float4x4	projectionMatrix;

	float4x4	wvpMatrix;

	float4x4	texMtx;
	
};
// Nonnumeric values cannot be added to a cbuffer.
Texture2D	gSpecMap;
Texture2D	gBlendMap;
Texture2D	gLayer1;
Texture2D	gLayer2;
Texture2D	gLayer3;

float  height1;
float  height2;
float  height3;
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
    float4 positionH	: SV_POSITION;
	float3 positionW	: POSITION;
	float4 normal		: NORMAL;
	float2 tiledUV      : TEXCOORD1;
    float2 stretchedUV  : TEXCOORD2;
	
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType TextureVertexShader(VertexInputType input){
    PixelInputType output;       

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.positionW = mul(float4(input.position, 1.0f), worldMatrix);
	output.normal	= mul(float4(input.normal, 0.0f), worldMatrix);	

	output.positionH = mul(float4(input.position, 1.0f), wvpMatrix);

	output.tiledUV     = 16*input.tex;
	output.stretchedUV = input.tex;

    return output;
}

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader for texturing based on height
////////////////////////////////////////////////////////////////////////////////
float4 TexturePixelShaderHeight(PixelInputType input) : SV_Target
{
	// Interpolating normal can make it not be of unit length so normalize it.
    float3 normalW = normalize(input.normal);

	float4 spec = gSpecMap.Sample( SampleType, input.tiledUV );
			
	// Map [0,1] --> [0,256]
	spec.a *= 256.0f;

	// Get materials from texture maps for diffuse col.		
	float4 c1 = gLayer1.Sample( SampleType, input.tiledUV );
	float4 c2 = gLayer2.Sample( SampleType, input.tiledUV );
	float4 c3 = gLayer3.Sample( SampleType, input.tiledUV );
	
    float height = input.positionW.y;
	float4 terrainColor;
	
	//compute terrain color based on height
	if (height >= height3){
		terrainColor = c3;
	}
	else if (height >= height2){
		float lerpVal = ( height - height2 ) / ( height3 - height2 );
		terrainColor = lerp( c2, c3, lerpVal );
	}
	else {
		float lerpVal = ( height - height1 ) / ( height2 - height1 );
		terrainColor = lerp( c1, c2, lerpVal );
	}

	// Compute the lit color for this pixel.
	SurfaceInfo v = {input.positionW, normalW, terrainColor, spec};
	float3 litColor;
	if( gLightType == 0 ) // Parallel
    {
		litColor = ParallelLight(v, gLight, gEyePosW); 
    }
    else if( gLightType == 1 ) // Point
    {
		litColor = PointLight(v, gLight, gEyePosW);
	}
	else // Spot
	{
		litColor = Spotlight(v, gLight, gEyePosW);
	}

	return float4(litColor, terrainColor.a);	
}

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader for texturing based on blend map
////////////////////////////////////////////////////////////////////////////////
float4 TexturePixelShaderBlendMap(PixelInputType input) : SV_Target
{
	// Interpolating normal can make it not be of unit length so normalize it.
    float3 normalW = normalize(input.normal);

	float4 spec = gSpecMap.Sample( SampleType, input.tiledUV );
			
	// Map [0,1] --> [0,256]
	spec.a *= 256.0f;

	// Get materials from texture maps for diffuse col.		
	float4 c1 = gLayer1.Sample( SampleType, input.tiledUV );
	float4 c2 = gLayer2.Sample( SampleType, input.tiledUV );
	float4 c3 = gLayer3.Sample( SampleType, input.tiledUV );
	
	float4 t = gBlendMap.Sample( SampleType, input.stretchedUV ); 
	
	// Find the inverse of all the blend weights so that we can  scale the total color to the range [0, 1].
	float totalInverse = 1.0f / (t.r + t.g + t.b);

	// Scale the colors by each layer by its corresponding weight
	// stored in the blendmap.  
	c1 *= t.r * totalInverse;
	c2 *= t.g * totalInverse;
	c3 *= t.b * totalInverse;

	float4 mixedColor = c1+c2+c3;	

	// Compute the lit color for this pixel.
	SurfaceInfo v = {input.positionW, normalW, mixedColor, spec};
	float3 litColor = ParallelLight(v, gLight, gEyePosW);

	return float4(litColor, mixedColor.a);	
	

}

////////////////////////////////////////////////////////////////////////////////
// Techniques
////////////////////////////////////////////////////////////////////////////////
technique10 TextureTechnique
{
    pass pass0
    {
        SetVertexShader(CompileShader(vs_4_0, TextureVertexShader()));
		SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, TexturePixelShaderHeight()));
        
    }
}
technique10 TextureTechniqueBlendMap
{
    pass pass0
    {
        SetVertexShader(CompileShader(vs_4_0, TextureVertexShader()));
		SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, TexturePixelShaderBlendMap()));
        
    }
}