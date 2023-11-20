#include "LightHelper.fx"
#include "GlobalShader.fx"

cbuffer streamTime
{
    float gTimeDelta = 0.f;
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
    float4 vProjPos : TEXCOORD2;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
};

VS_OUT VS_MAIN(VS_IN input)
{
    VS_OUT Out = (VS_OUT) 0;
    
    matrix WVP = ComputeTransformMatrix(W, V, P);
    
    Out.position = mul(float4(input.position, 1.f), WVP);
    Out.texcoord = input.texcoord;
    Out.normal = normalize(mul(float4(input.normal, 0.f), W));
    Out.worldPos = mul(float4(input.position, 1.f), W);
    Out.vProjPos = Out.position;
    Out.vTangent = normalize(mul(input.vTangent, W)).xyz;
    Out.vBinormal = normalize(cross(Out.normal.xyz, Out.vTangent));
    
    
    return Out;
}

VS_OUT VS_MAIN_WATER(VS_IN input)
{
    VS_OUT Out = (VS_OUT) 0;
    
    matrix WVP = ComputeTransformMatrix(W, V, P);
    
    Out.position = mul(float4(input.position, 1.f), WVP);
    Out.texcoord = input.texcoord.xy + gTimeDelta;
    Out.normal = normalize(mul(float4(input.normal, 0.f), W));
    Out.worldPos = mul(float4(input.position, 1.f), W);
    Out.vProjPos = Out.position;
    Out.vTangent = normalize(mul(input.vTangent, W)).xyz;
    Out.vBinormal = normalize(cross(Out.normal.xyz, Out.vTangent));
    
    
    return Out;
}


struct PS_IN
{
    float4 position : SV_POSITION;
    float4 normal   : NORMAL;
    float2 texcoord : TEXCOORD0;
    float4 worldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    float3 vTangent : TANGENT;
    float3 vBinormal : BINORMAL;
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
    vector vNormalDesc = NormalMap.Sample(LinearSampler, input.texcoord);
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
    float3x3 WorldMatrix = float3x3(input.vTangent, input.vBinormal, input.normal.xyz);
    
    vNormal = normalize(mul(vNormal, WorldMatrix));
    
    if (vMtrlDiffuse.a < 0.3f)
        discard;
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(input.vProjPos.z / input.vProjPos.w, input.vProjPos.w / fCameraFar, 0.f, 0.f);

    
    return Out;
}

PS_OUT PS_WATER_MAIN(PS_IN input)
{
    PS_OUT Out = (PS_OUT) 0;

    //vector vMtrlDiffuse = DiffuseMap.Sample(LinearSampler, input.texcoord);
    vector vNormalDesc = NormalMap.Sample(LinearSampler, input.texcoord * 10.f);
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
    float3x3 WorldMatrix = float3x3(input.vTangent, input.vBinormal, input.normal.xyz);
    
    vNormal = normalize(mul(vNormal, WorldMatrix));

    
    Out.vDiffuse = vector(0.08f, 0.08f, 0.1f, 0.2f);
    Out.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(input.vProjPos.z / input.vProjPos.w, input.vProjPos.w / fCameraFar, 0.f, 0.f);

    
    return Out;


    
    return Out;
}

technique11 MeshTechnique
{
    SOLID_PASS_VP(StaticMesh, VS_MAIN, PS_MAIN)
    SOLID_PASS_VP(WaterMesh, VS_MAIN_WATER, PS_WATER_MAIN)
}