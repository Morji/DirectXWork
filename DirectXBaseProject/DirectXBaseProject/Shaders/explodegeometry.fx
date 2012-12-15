
#include "lighthelper.fx"

//////////////
// TYPEDEFS //
//////////////
cbuffer cbPerFrame{
	Light	gLight;
	int		gLightType;
	float3	gEyePosW;

	float	timeStep;
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
    float4 positionH : SV_POSITION;
	float3 positionW : POSITION;
	float4 normal	 : NORMAL;
    float4 color	 : COLOR0;
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType ColorVertexShader(VertexInputType input)
{
	PixelInputType output;
    // Calculate the position of the vertex against the world, view, and projection matrices
	output.positionH = float4(input.position, 1.0f);
    output.normal	= mul(float4(input.normal, 0.0f), worldMatrix);

	// Store the texture coordinates for the pixel shader.
	output.positionW = input.position;
    // Store the input color for the pixel shader to use.
    output.color = input.color;
    
    return output;
}

//Prototype any necessary functions
float3 GetFaceNormal(float3 a, float3 b, float3 c){
	float3 ab = a-b;
	float3 cb = c-b;

	//float random = noise(ab);
	//float3 randomVec = float3(random,random,random);
	//ab.x = random + ab.x;

	return cross(ab,cb);
}



////////////////////////////////////////////////////////////////////////////////
// Geometry Shader
////////////////////////////////////////////////////////////////////////////////
[maxvertexcount(3)]
void ColorGeometryShader(triangle PixelInputType input[3],inout TriangleStream<PixelInputType> triStream)
{
	PixelInputType output;
	float3 faceNormal = GetFaceNormal(input[0].positionW,input[1].positionW,input[2].positionW);
	for (int i = 0; i < 3; i++){
		output.positionH = input[i].positionH - float4(faceNormal,0)*timeStep;
		output.positionH = mul(output.positionH, wvpMatrix);
		output.positionW = mul(float4(input[i].positionW, 1.0f), worldMatrix);
		output.normal = input[i].normal;
		output.color = input[i].color;
		triStream.Append(output);
	}	
	triStream.RestartStrip();
}

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 ColorPixelShader(PixelInputType input) : SV_Target
{
	// Interpolating normal can make it not be of unit length so normalize it.
    float3 normalW = normalize(input.normal);

	// Compute the lit color for this pixel.
	SurfaceInfo v = {input.positionW, normalW, input.color, input.color};
	float3 litColor = ParallelLight(v, gLight, gEyePosW);

	return float4(litColor, 1.0f);	
}

technique10 ExplodeTechnique
{
    pass pass0
    {
        SetVertexShader(CompileShader(vs_4_0, ColorVertexShader()));
		SetGeometryShader(CompileShader(gs_4_0, ColorGeometryShader()));
        SetPixelShader(CompileShader(ps_4_0, ColorPixelShader()));
    }
}
