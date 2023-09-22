cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix, viewMatrix, projMatrix;
}

cbuffer camera : register(b1)
{
    vector camPosition;
}

struct VS_IN
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
};


struct VS_OUT
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
    float4 worldPosition : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN input)
{
    VS_OUT output;
    
    matrix matWV, matWVP;
    
    matWV = mul(worldMatrix, viewMatrix);
    matWVP = mul(matWV, projMatrix);
    
    output.position = mul(float4(input.position, 1.f), matWVP);
    output.uv = input.uv;
    output.normal = mul(float4(input.normal, 0.f), worldMatrix);
    output.worldPosition = mul(float4(input.position, 1.f), worldMatrix);
    
    return output;
}


struct PS_IN
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
    float4 worldPosition : TEXCOORD1;
};

struct PS_OUT
{
    float4 Color : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN vertexin)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.Color = float4(1.f, 1.f, 1.f, 1.f);
    
    return Out;
}

technique11 WireframeTechnique
{
    pass EditTerrain
    {
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}