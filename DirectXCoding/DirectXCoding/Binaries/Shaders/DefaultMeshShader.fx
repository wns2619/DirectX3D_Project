#include "LightHelper.fx"

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix  : packoffset(c0);
    matrix viewMatrix   : packoffset(c4);
    matrix projMatrix   : packoffset(c8);
};

cbuffer LightBuffer : register(b0)
{
    LightDesc Lightinfo : packoffset(c0);
};

cbuffer Material : register(b1)
{
    vector materialAmbient = vector(0.4f, 0.4f, 0.4f, 0.4f);
    vector materialSpecular = vector(1.f, 1.f, 1.f, 1.f);
};

struct VS_IN
{
    float3 position : POSITION;
    float3 normal   : NORMAL;
    float2 texcoord : TEXCOORD0;
    float3 vTangent : TANGENT;
};

struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 normal   : NORMAL;
    float2 texcoord : TEXCOORD0;
    float4 worldPos : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN input)
{
    VS_OUT Out = (VS_OUT) 0;
    
    matrix WVP = ComputeTransformMatrix(worldMatrix, viewMatrix, projMatrix);
    
    Out.position = mul(float4(input.position, 1.f), WVP);
    Out.texcoord = input.texcoord;
    Out.normal = mul(float4(input.normal, 0.f), worldMatrix);
    Out.worldPos = mul(float4(input.position, 1.f), worldMatrix);
    
    return Out;
}


struct PS_IN
{
    float4 position : SV_POSITION;
    float4 normal   : NORMAL;
    float2 texcoord : TEXCOORD0;
    float4 worldPos : TEXCOORD1;
};

struct PS_OUT
{
    float4 color : SV_TARGET0;
};

Texture2D DiffuseTexture : register(t0);

PS_OUT PS_MAIN(PS_IN input)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMaterialdiffuse = DiffuseTexture.Sample(LinearSampler, input.texcoord);
    
    if (vMaterialdiffuse.a < 0.3f)
        discard;
    
    vector shade = max(dot(normalize(-Lightinfo.Direction), normalize((float3)input.normal)), 0.f) +
    Lightinfo.Ambient * materialAmbient;
    
    vector _reflect = reflect(normalize(float4(Lightinfo.Direction, 1.f)), normalize(input.normal));
    vector _look = input.worldPos - float4(CameraPosition(), 1.f);
    
    float _specular = pow(max(dot(normalize(-_look), normalize(_reflect)), 0.f), 30.f);
    
    Out.color = (Lightinfo.Diffuse * vMaterialdiffuse) * saturate(shade) +
    (Lightinfo.Specular * materialSpecular) * _specular;
    
    return Out;
}

technique11 MeshTechnique
{
    pass Mesh
    {
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = null;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}