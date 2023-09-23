#include "LightHelper.fx"

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix, viewMatrix, projMatrix;
};


cbuffer LightBuffer : register(b1)
{
    DirectionalLight dirLight;
}

cbuffer Material : register(b2)
{
    vector materialAmbient   = vector(0.4f, 0.4f, 0.4f, 1.f);
    vector materialSpecular  = vector(1.f, 1.f, 1.f, 1.f);
}

cbuffer camera : register(b3)
{
    vector camPosition;
}

struct VS_IN
{
    float3 position : POSITION;
    float3 normal   : NORMAL;
    float2 uv       : TEXCOORD0;
   
};

struct VS_OUT
{
    float4 position : SV_POSITION;
    float3 normal   : NORMAL;
    float2 uv       : TEXCOORD0;
    float4 worldPos : TEXCOORD1;
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
    output.worldPos = mul(float4(input.position, 1.f), worldMatrix);
    
    
    return output;
}

struct PS_IN
{
    float4 postion  : SV_POSITION;
    float3 noraml   : NORMAL;
    float2 uv       : TEXCOORD0;
    float4 worldPos : TEXCOORD1;
};

struct PS_OUT 
{
    float4 Color : SV_TARGET0;
};

SamplerState LinearSampler : register(s0)
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = wrap;
    AddressV = wrap;

};

SamplerState PointSampler : register(s1)
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = wrap;
    AddressV = wrap;
   
};

Texture2D DiffuseTexture : register(t0);


PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector mtldff = DiffuseTexture.Sample(LinearSampler, In.uv * 30.f);
    
    vector shader = max(dot(normalize(-dirLight.Direction.xyz), normalize(In.noraml)), 0.f) + dirLight.Ambient * materialAmbient;
    
    float3 reflectDirection = reflect(normalize(dirLight.Direction.xyz), normalize(In.noraml));
    float3 look = In.worldPos - camPosition;
    
    float specular = pow(max(dot(normalize(-look), normalize(reflectDirection)), 0.f), 30.f);
    
    Out.Color = (dirLight.Diffuse * mtldff) * saturate(shader) + (dirLight.Specular * materialSpecular) * specular;
    
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