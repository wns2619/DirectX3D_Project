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

    //vector vMaterialdiffuse = DiffuseMap.Sample(LinearSampler, input.texcoord);
    ////vector vMaterialdiffuse = vector(1.f, 1.f, 1.f, 1.f);
    
    //if (vMaterialdiffuse.a < 0.3f)
    //    discard;
    
    //vector shade = max(dot(normalize(-GlobalLight.Direction), normalize((float3) input.normal)), 0.f) +
    //GlobalLight.Ambient * Material.Ambient;
    
    //vector _reflect = reflect(normalize(float4(GlobalLight.Direction, 1.f)), normalize(input.normal));
    //vector _look = input.worldPos - float4(CameraPosition(), 1.f);
    
    //float _specular = pow(max(dot(normalize(-_look), normalize(_reflect)), 0.f), 30.f);
    
    //Out.color = (GlobalLight.Diffuse * vMaterialdiffuse) * saturate(shade) +
    //(GlobalLight.Specular * Material.Specular) * _specular;
    
    //Out.color = ComputeLight(input.normal.xyz, input.texcoord, input.worldPos.xyz);
    Out.color = ComputeTeacherLight(input.normal, input.texcoord, input.worldPos);
    
    return Out;
}

technique11 MeshTechnique
{
    SOLID_PASS_VP(StaticMesh, VS_MAIN, PS_MAIN)
}