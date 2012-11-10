//=============================================================================
// lighting.fx by Frank Luna (C) 2008 All Rights Reserved.
//
// Transforms and lights geometry.
//=============================================================================

#include "lighthelper.fx"
 
cbuffer cbPerFrame{

	Light gLight;
	int gLightType; 
	float3 gEyePosW;
	
};
cbuffer cbPerObject{

	float4x4 worldMatrix;
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
};
struct VertexInputType{

	float3 posL    : POSITION;
	float3 normalL : NORMAL;
	float4 diffuse : DIFFUSE;
	float4 spec    : SPECULAR;
};
struct PixelInputType{

	float4 posH    : SV_POSITION;
    float3 posW    : POSITION;
    float3 normalW : NORMAL;
    float4 diffuse : DIFFUSE;
    float4 spec    : SPECULAR;
};

PixelInputType VertexInputShader(VertexInputType vIn){

	PixelInputType vOut;
	
	// Transform to world space space.
	vOut.posW    = mul(float4(vIn.posL,1.0f), worldMatrix);
	vOut.normalW = mul(float4(vIn.normalL, 0.0f), worldMatrix);	
		
	// Calculate the position of the vertex against the world, view, and projection matrices.
	// Transform to homogeneous clip space.
	vOut.posH = mul(float4(vIn.posL,1.0f), worldMatrix);
    vOut.posH = mul(vOut.posH, viewMatrix);
    vOut.posH = mul(vOut.posH, projectionMatrix);
	
	// Output vertex attributes for interpolation across triangle.
	vOut.diffuse = vIn.diffuse;
	vOut.spec    = vIn.spec;
	
	return vOut;
}
 
float4 PixelInputShader(PixelInputType pIn) : SV_Target{

	// Interpolating normal can make it not be of unit length so normalize it.
    pIn.normalW = normalize(pIn.normalW);   
   
    SurfaceInfo v = {pIn.posW, pIn.normalW, pIn.diffuse, pIn.spec};
    
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
	   
    return float4(litColor, pIn.diffuse.a);
}

technique10 LightTech{

    pass P0{

        SetVertexShader( CompileShader( vs_4_0, VertexInputShader() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PixelInputShader() ) );
    }
}



