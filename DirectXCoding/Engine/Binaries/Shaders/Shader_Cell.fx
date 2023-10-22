cbuffer PipeLineBuffer
{
    Matrix W, V, P;
};

cbuffer LineColor
{
    vector vLineColor = vector(0.f, 1.f, 0.f, 1.f);
    float fHeight = 0.f;
};

struct VS_IN
{
    float3 vPosition : POSITION;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    matrix matWV, matWVP;
    
    vector vPosition = mul(float4(In.vPosition, 1.f), W);
    vPosition.y += fHeight;
    
    vPosition = mul(vPosition, V);
    vPosition = mul(vPosition, P);
    
    Out.vPosition = vPosition;
    
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = vLineColor;
    
    return Out;
}


technique11 DefaultTechnique
{
    pass Cell
    {
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}
