#include "GlobalShader.fx"
#include "LightHelper.fx"

vector NewColor = vector(1.f, 1.f, 1.f, 1.f);

Texture2D<vector> UITexture : register(t0);


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


struct GS_IN
{
    float4 vPosition : POSITION;
    float2 vTexCoord : TEXCOORD;
};

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

PS_OUT PS_MAIN(VertexOut input)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = DiffuseMap.Sample(LinearSampler, input.uv);
    
    if (vMtrlDiffuse.a < 0.3f)
        discard;
    
    Out.vDiffuse = vMtrlDiffuse * NewColor;
    //vector vNormal = NormalMap.Sample(LinearSampler, input.uv);
    Out.vNormal = vector(input.normal.xyz * 0.5f + 0.5f, 0.f);
    
    Out.vDepth = vector(input.vProjPos.z / input.vProjPos.w, input.vProjPos.w / fCameraFar, 0.f, 0.f);
        
    
    return Out;
}

struct PS_UI_OUT
{
    float4 vColor : SV_TARGET0;
};

PS_UI_OUT PS_UI(VertexOut input)
{
    PS_UI_OUT Out = (PS_UI_OUT) 0;
    
    vector vMtrlDiffuse = UITexture.Sample(LinearSampler, input.uv);
    
    //if (vMtrlDiffuse.a < 0.3f)
    //    discard;
    
    Out.vColor = vMtrlDiffuse;
    //Out.vNormal = vector(input.normal.xyz * 0.5 + 0.5f, 0.f);
    //Out.vDepth = vector(input.vProjPos.z / input.vProjPos.w, input.vProjPos.w / fCameraFar, 0.f, 0.f);
    
    return Out;
}

technique11 DefaultTechnique
{
    SOLID_PASS_VP(Rect, VS_MAIN, PS_MAIN)
    SOLID_PASS_VP(UI, VS_MAIN, PS_UI)
}