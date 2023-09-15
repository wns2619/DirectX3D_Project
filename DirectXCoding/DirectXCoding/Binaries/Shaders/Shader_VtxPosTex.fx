
cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix ProjMatrix;
};

struct VS_IN
{
    float3 postion : POSITION;
    float2 uv : TEXCOORD0;
};

struct VS_OUT
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD0;
};


VS_OUT VS_MAIN(VS_IN input)
{
    VS_OUT output;
    
    matrix matWV, matWVP;
    
    matWV = mul(worldMatrix, viewMatrix);
    matWVP = mul(matWV, ProjMatrix);
    
    output.position = mul(float4(input.postion, 1.f), matWVP);
    output.uv = input.uv;

    return output;
}

struct PS_IN
{
    float4 postion : SV_Position;
    float2 uv : TEXCOORD0;
};

struct PS_OUT
{
    float4 Color : SV_Target0;
};


sampler LinearSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
};

sampler PointSampler = sampler_state
{
    Filter = MIN_MAG_MIP_POINT;
};

Texture2D ShadersTexture;
Texture2D ShadersTextures[2];

PS_OUT PS_MAIN(PS_IN input)
{
    PS_OUT Out = (PS_OUT)0;
   
    vector sourColor = ShadersTextures[0].Sample(PointSampler, input.uv);
    vector destColor = ShadersTextures[1].Sample(PointSampler, input.uv);
    
    Out.Color = sourColor + destColor;
    
    return Out;
}

technique11 DefaultTechnique
{
    pass UI
    {
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();

    }
}