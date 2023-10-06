#include "GlobalShader.fx"
#include "LightHelper.fx"

VertexOut VS_MAIN(VertexTextureNormal input)
{
    VertexOut output;
    
    matrix matWVP = ComputeTransformMatrix(W, V, P);
    
    output.position = mul(float4(input.position, 1.f), matWVP);
    output.uv = input.uv;
    output.normal = mul(float4(input.normal, 0.f), W);
    output.worldPosition = mul(float4(input.position, 1.f), W);
    
    return output;
}


PixelOut PS_MAIN(PixelIn vertexin)
{
    PixelOut Out = (PixelOut) 0;
    
    Out.Color = float4(1.f, 1.f, 1.f, 1.f);
    
    return Out;
}


technique11 WireframeTechnique
{
    WIRE_PASS_VP(WireTerrain, VS_MAIN, PS_MAIN)
}