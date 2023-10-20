#include "GlobalShader.fx"
#include "LightHelper.fx"



VertexOut VS_MAIN(VertexTextureNormal input)
{
    VertexOut output = (VertexOut) 0;
    
    
    Matrix WVP = ComputeTransformMatrix(W, V, P);
    
    output.position = mul(float4(input.position, 1.f), WVP);
    output.uv = input.uv;
    output.normal = mul(float4(input.normal, 0.f), W);
    output.worldPosition = mul(float4(input.position, 1.f), W);
    
    return output;
}

PixelOut PS_MAIN(VertexOut input)
{
    PixelOut Out = (PixelOut) 0;
   
    Out.Color = ComputeTeacherLight(input.normal, input.uv, input.worldPosition.xyz);
    
    return Out;
}

technique11 DefaultTechnique
{
    SOLID_PASS_VP(Rect, VS_MAIN, PS_MAIN)
}