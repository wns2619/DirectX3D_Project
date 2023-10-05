#include "LightHelper.fx"



cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix  : packoffset(c0);
    matrix viewMatrix   : packoffset(c4);
    matrix projMatrix   : packoffset(c8);
    // 내 현재 모델의 전체 뼈가 아니라, 이 메시가 사용하는 뼈의 정보만 불러온다.
    matrix BoneMatrices[256] : packoffset(c12);
};

cbuffer LightBuffer : register(b0)
{
    Light Lightinfo : packoffset(c0);
};

cbuffer Material : register(b1)
{
    vector materialAmbient = vector(0.4f, 0.4f, 0.4f, 0.4f);
    vector materialSpecular = vector(1.f, 1.f, 1.f, 1.f);
};

cbuffer camera : register(b2)
{
    vector cameraPosition : packoffset(c0);
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
struct VS_IN
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD0;
    float3 tangent : TANGENT;
    float3 bitTangent : BITTANGENT;
    
    uint4   blendIndices : BLENDINDEX;
    float4  blendWeights : BLENDWEIGHT;
};

struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 normal : NORMAL;
    float2 texcoord : TEXCOORD0;
    float4 worldPos : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN input)
{
    VS_OUT Out = (VS_OUT) 0;
    
    matrix BoneMatrix = BoneMatrices[input.blendIndices.x] * input.blendWeights.x +
     BoneMatrices[input.blendIndices.y] * input.blendWeights.y +
     BoneMatrices[input.blendIndices.z] * input.blendWeights.z +
     BoneMatrices[input.blendIndices.w] * input.blendWeights.w;
    
    //matrix BoneMatrix = BoneMatrices[input.blendIndices.x][0][0] * input.blendWeights.x +
    //BoneMatrices[input.blendIndices.y][0][0] * input.blendWeights.y +
    //BoneMatrices[input.blendIndices.z][0][0] * input.blendWeights.z +
    //BoneMatrices[input.blendIndices.w][0][0] * input.blendWeights.w;
    
    vector bonePosition = mul(float4(input.position, 1.f), BoneMatrix);
    
    matrix WVP = ComputeTransformMatrix(worldMatrix, viewMatrix, projMatrix);
    
    Out.position = mul(bonePosition, WVP);
    Out.texcoord = input.texcoord;
    Out.normal = mul(float4(input.normal, 0.f), worldMatrix);
    Out.worldPos = mul(float4(input.position, 1.f), worldMatrix);
    
    return Out;
}


struct PS_IN
{
    float4 position : SV_POSITION;
    float4 normal : NORMAL;
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
    
    vector shade = max(dot(normalize(float4(-Lightinfo.Direction, 1.f)), normalize(input.normal)), 0.f) +
    Lightinfo.Ambient * materialAmbient;
    
    vector _reflect = reflect(normalize(float4(Lightinfo.Direction, 1.f)), normalize(input.normal));
    vector _look = input.worldPos - cameraPosition;
    
    float _specular = pow(max(dot(normalize(-_look), normalize(_reflect)), 0.f), 30.f);
    
    Out.color = (float4(Lightinfo.Diffuse, 1.f) * vMaterialdiffuse) * saturate(shade) +
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