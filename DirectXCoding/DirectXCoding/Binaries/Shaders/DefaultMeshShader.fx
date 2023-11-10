#include "LightHelper.fx"
#include "GlobalShader.fx"

cbuffer CameraCBuffer
{
    float fCameraFar = 50.f;
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
    float3 vTangent : TANGENT;
    float4 worldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

VS_OUT VS_MAIN(VS_IN input)
{
    VS_OUT Out = (VS_OUT) 0;
    
    matrix WVP = ComputeTransformMatrix(W, V, P);
    
    Out.position = mul(float4(input.position, 1.f), WVP);
    Out.texcoord = input.texcoord;
    Out.normal = normalize(mul(float4(input.normal, 0.f), W));
    Out.vTangent = normalize(mul(input.vTangent, W)).xyz;
    Out.worldPos = mul(float4(input.position, 1.f), W);
    Out.vProjPos = Out.position;
    
    return Out;
}


struct PS_IN
{
    float4 position : SV_POSITION;
    float4 normal   : NORMAL;
    float2 texcoord : TEXCOORD0;
    float3 vTangent : TANGENT;
    float4 worldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
};

PS_OUT PS_MAIN(PS_IN input)
{
    PS_OUT Out = (PS_OUT)0;

    vector vMtrlDiffuse = DiffuseMap.Sample(LinearSampler, input.texcoord);
    
    if (vMtrlDiffuse.a < 0.3f)
        discard;
    
    Out.vDiffuse = vMtrlDiffuse;
    //ComputeNormalMapping(input.normal.xyz, input.vTangent, input.texcoord);
    Out.vNormal = vector(input.normal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(input.vProjPos.z / input.vProjPos.w, input.vProjPos.w / fCameraFar, 0.f, 0.f);


    
    return Out;
}

technique11 MeshTechnique
{
    SOLID_PASS_VP(StaticMesh, VS_MAIN, PS_MAIN)
}