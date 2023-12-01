#include "GlobalShader.fx"

Texture2D<vector> UITexture : register(t0);
Texture2D<vector> UITextureArray0[4];

float fTime = 0.f;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    Matrix WVP = ComputeTransformMatrix(W, V, P);
    
    Out.vPosition = mul(float4(In.vPosition, 1.f), WVP);
    Out.vTexcoord = In.vTexcoord;
    
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vMtrlDiffuse = UITexture.Sample(LinearSampler, In.vTexcoord);
        
    if (vMtrlDiffuse.a <= 0.1f)
        discard;
    
    Out.vColor = vMtrlDiffuse;
    
    return Out;
}

PS_OUT PS_UI_ARRAY(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vMtrlDiffuse = vector(1.f, 1.f, 1.f, 1.f);
    
    if (fTime < 1.f)
        vMtrlDiffuse = UITextureArray0[0].Sample(LinearSampler, In.vTexcoord);
    else if (fTime < 2.f)
        vMtrlDiffuse = UITextureArray0[1].Sample(LinearSampler, In.vTexcoord);
    else if (fTime < 3.f)
        vMtrlDiffuse = UITextureArray0[2].Sample(LinearSampler, In.vTexcoord);
    else
    {
        vMtrlDiffuse = UITextureArray0[3].Sample(LinearSampler, In.vTexcoord);
        vMtrlDiffuse.a = lerp(vMtrlDiffuse.a, 0.f, fTime * 0.08f);
    }
    
    
    Out.vColor = vMtrlDiffuse;
    
    return Out;
}

technique11 UITechnique
{
    SOLID_PASS_VP(UI, VS_MAIN, PS_MAIN)
    SOLID_PASS_VP(UIArray, VS_MAIN, PS_UI_ARRAY)
}