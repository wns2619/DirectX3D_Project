#include "EngineShaderDefine.fx"

cbuffer TransformMatrix : register(b0)
{
    matrix W, V, P;
};

Texture2D gTexture : register(t0);

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



VS_OUT VS_MAIN( /* ���� */VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
	
    matrix matWV, matWVP;

    matWV = mul(W, V);
    matWVP = mul(matWV, P);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);

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


PS_OUT PS_MAIN_DEBUG(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = gTexture.Sample(LinearSampler, In.vTexcoord);
	
    return Out;
}

technique11 DefaultTechnique
{
    pass Target_Debug
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
    }
}




