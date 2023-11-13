#include "GlobalShader.fx"
#include "LightHelper.fx"

vector NewColor = vector(1.f, 1.f, 1.f, 1.f);



VertexOut VS_MAIN(VertexTextureNormal input)
{
    VertexOut output = (VertexOut) 0;
    
    
    Matrix WVP = ComputeTransformMatrix(W, V, P);
    
    output.position = mul(float4(input.position, 1.f), WVP);
    output.uv = input.uv;
    output.normal = mul(float4(input.normal, 0.f), W);
    output.worldPosition = mul(float4(input.position, 1.f), W);
    output.vProjPos = output.position;
    
    return output;
}

struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
};

PS_OUT PS_MAIN(VertexOut input)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = DiffuseMap.Sample(LinearSampler, input.uv);
    
    if (vMtrlDiffuse.a < 0.3f)
        discard;
    
    Out.vDiffuse = vMtrlDiffuse;
    //vector vNormal = NormalMap.Sample(LinearSampler, input.uv);
    Out.vNormal = vector(input.normal.xyz * 0.5f + 0.5f, 0.f);
    
    Out.vDepth = vector(input.vProjPos.z / input.vProjPos.w, input.vProjPos.w / fCameraFar, 0.f, 0.f);
        
    return Out;
}

technique11 DefaultTechnique
{
    SOLID_PASS_VP(Rect, VS_MAIN, PS_MAIN)
}