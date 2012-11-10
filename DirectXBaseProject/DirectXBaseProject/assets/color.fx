//=============================================================================
// color.fx by Frank Luna (C) 2008 All Rights Reserved.
//
// Transforms and colors geometry.
//=============================================================================

cbuffer cbPerObject
{
	float4x4 gWVP; 
	
	//float4 Color;
};

//RasterizerState myRS { FillMode = Solid; CullMode = None;};

void VS(float3 iPosL  : POSITION, out float4 oPosH  : SV_POSITION)
{
	// Transform to homogeneous clip space.
	oPosH = mul(float4(iPosL, 1.0f), gWVP);	
	//Color = Color;
}

float4 PS(float4 posH  : SV_POSITION ) : SV_Target
{ 
    //return a magenta colour  
    return float4(0.9f,0.3f,0.1f,1);
}

technique10 ColorTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
		//SetRasterizerState( myRS );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );		
    }
}
