#include "LightHelper.fx"
#include "GlobalShader.fx"

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
    
    matrix WVP = ComputeTransformMatrix(W, V, P);
    
    Out.position = mul(float4(input.position, 1.f), WVP);
    Out.texcoord = input.texcoord;
    Out.normal = mul(float4(input.normal, 0.f), W);
    Out.worldPos = mul(float4(input.position, 1.f), W);
    
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

PS_OUT PS_MAIN(PS_IN input)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.color = ComputeTeacherLight(input.normal, input.texcoord, input.worldPos);
    
    //Out.color = float4(CalcAmbient(input.normal, input.texcoord), 1.f);
    
    //Out.color += CalcDirectional(input.worldPos, input.texcoord, input.normal);
    
    //float2 fCenter = float2(0.5, 0.5); // 화면 중심의 정규화된 좌표
    //float strength = 3.0f;

    //float2 uv = input.texcoord; // 정규화된 텍스처 좌표

    //// 비네트 효과 적용 (화면 전체에)
    //float distanceFromCenter = distance(uv, fCenter);
    //float4 vignetteColor = 1.0 - saturate(strength * distanceFromCenter * distanceFromCenter);

    
    //Out.color.rgb = lerp(Out.color.rgb, Out.color.rgb * vignetteColor.rgb, vignetteColor.a * 2.0);
    
    
    return Out;
}

technique11 MeshTechnique
{
    SOLID_PASS_VP(StaticMesh, VS_MAIN, PS_MAIN)
}