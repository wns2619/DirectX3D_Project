cbuffer MatrixBuffer
{
    matrix worldMatrix, viewMatrix, projMatrix;
};

struct VS_IN
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
};

struct VS_OUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};


VS_OUT VS_MAIN(VS_IN input)
{
    VS_OUT output;
    
    matrix matWV, matWVP;
    
    matWV = mul(worldMatrix, viewMatrix);
    matWVP = mul(matWV, projMatrix);
    
    output.position = mul(float4(input.position, 1.f), matWVP);
    output.uv = input.uv;
    
    
    return output;
}

struct PS_IN
{
    float4 postion : SV_POSITION;
    float2 uv : TEXCOORD0;
};

struct PS_OUT 
{
    float4 Color : SV_TARGET0;
};

sampler LinearSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = wrap;
    AddressV = wrap;

};

sampler PointSampler = sampler_state
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = wrap;
    AddressV = wrap;
   
};

Texture2D ShadersTexture;


PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.Color = ShadersTexture.Sample(LinearSampler, In.uv * 30.f);

    return Out;
}

technique11 DefalutTechnique
{
    pass Terrain
    {
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = null;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}