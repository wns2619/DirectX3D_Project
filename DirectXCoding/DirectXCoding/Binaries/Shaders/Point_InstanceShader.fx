#include "GlobalShader.fx"

Texture2D gTexture;

cbuffer CameraBuffer
{
    float4 CameraPos;
};

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vPSize : PSIZE;
    
    float4 vRight : WORLD0;
    float4 vUp : WORLD1;
    float4 vLook : WORLD2;
    float4 vTranslation : WORLD3;
};

struct VS_OUT
{
    float4 vPosition : POSITION;
    float2 vPSize : PSIZE;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    matrix TransformMatrix = float4x4(normalize(In.vRight), normalize(In.vUp), normalize(In.vLook), In.vTranslation);
    
    vector vPosition = mul(float4(In.vPosition, 1.f), TransformMatrix);
    
    Out.vPosition = mul(vPosition, W);
    Out.vPSize = float2(In.vPSize.x * length(In.vRight), In.vPSize.y * length(In.vUp));

    return Out;
}

struct GS_IN
{
    float4 vPosition : POSITION;
    float2 vPSize : PSIZE;
};

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

[maxvertexcount(20)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> OutStream)
{
    GS_OUT Out[4];
    
    float3 vLook = CameraPos - In[0].vPosition.xyz;
    float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook.xyz)) * In[0].vPSize.x * 0.5f;
    float3 vUp = normalize(cross(vLook.xyz, vRight.xyz)) * In[0].vPSize.y * 0.5f;

    matrix matVP;
    
    matVP = mul(V, P);
    
    Out[0].vPosition = vector(In[0].vPosition.xyz + vRight + vUp, 1.f);
    Out[0].vPosition = mul(Out[0].vPosition, matVP);
    Out[0].vTexcoord = float2(0.0f, 0.f);

    Out[1].vPosition = vector(In[0].vPosition.xyz - vRight + vUp, 1.f);
    Out[1].vPosition = mul(Out[1].vPosition, matVP);
    Out[1].vTexcoord = float2(1.0f, 0.f);

    Out[2].vPosition = vector(In[0].vPosition.xyz - vRight - vUp, 1.f);
    Out[2].vPosition = mul(Out[2].vPosition, matVP);
    Out[2].vTexcoord = float2(1.0f, 1.0f);

    Out[3].vPosition = vector(In[0].vPosition.xyz + vRight - vUp, 1.f);
    Out[3].vPosition = mul(Out[3].vPosition, matVP);
    Out[3].vTexcoord = float2(0.0f, 1.0f);
    
    OutStream.Append(Out[0]);
    OutStream.Append(Out[1]);
    OutStream.Append(Out[2]);
    OutStream.RestartStrip();

    OutStream.Append(Out[0]);
    OutStream.Append(Out[2]);
    OutStream.Append(Out[3]);
    OutStream.RestartStrip();
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
    
    vector vMtrlDiffuse = gTexture.Sample(PointSampler, In.vTexcoord);
    //Out.vColor.rgb = float3(1.f, 0.f, 0.f);
    
 
    
    vMtrlDiffuse.a *= vMtrlDiffuse.b;
    
    //if(vMtrlDiffuse.a <= 0.3f)
    //    discard;
    
    Out.vColor = vMtrlDiffuse;
    
    
    return Out;
}

technique11 DefaultTechnique
{
	/* */
    pass Particle
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

		/* 여러 셰이더에 대해서 각각 어떤 버젼으로 빌드하고 어떤 함수를 호출하여 해당 셰이더가 구동되는지를 설정한다. */
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}