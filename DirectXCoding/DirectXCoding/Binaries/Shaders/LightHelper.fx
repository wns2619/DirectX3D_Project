#ifndef _LIGHTHELPER_FX_
#define _LIGHTHELPER_FX_

#include "GlobalShader.fx"

// struct

struct DirectionalLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float4 Direction;
    float pad;
};

struct PointLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    
    float3 Position;
    float Range;
    
    float3 Att;
    float apd;
};

struct SpotLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    
    float3 Position;
    float Range;
    
    float3 Direction;
    float Spot;
    
    float3 Att;
    float pad;
};

struct LightDesc
{
    float4 Position;
    float4 Emissive;
    float4 Diffuse;
    float4 Ambient;
    float4 Specular;
    
    float3 Direction;
    float1 intensity;
    
    float2 spotAngles;
    int    type;
    float1 range;
    
    float3 rotationDeg;
    bool   enabled;
};

struct MaterialDesc
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float4 Emissive;
    float4 Reflect;
    
};

// global Buffer

cbuffer LightBuffer
{
    LightDesc GlobalLight;
};

cbuffer MaterialBuffer
{
    MaterialDesc Material;
};

// ShaderResourceVIEW

Texture2D DiffuseMap;
Texture2D SpecularMap;
Texture2D NormalMap;

// Function

matrix ComputeTransformMatrix(matrix world, matrix view, matrix proj)
{
    matrix matWV, matWVP;
    
    matWV = mul(world, view);
    matWVP = mul(matWV, proj);
    
    return matWVP;
};

float4 ComputeLight(float3 normal, float2 uv, float3 worldPosition)
{
    float4 ambientColor = 0;
    float4 diffuseColor = 0;
    float4 specularColor = 0;
    float4 emissiveColor = 0;
    
    // Ambient
    {
        float4 color = GlobalLight.Ambient * Material.Ambient;
        ambientColor = DiffuseMap.Sample(LinearSampler, uv) * color;
    }
    
    // Diffuse
    {
        float4 color = DiffuseMap.Sample(LinearSampler, uv);
        float value = dot(-GlobalLight.Direction, normalize(normal));
        diffuseColor = color * value * GlobalLight.Diffuse * Material.Diffuse;
    }
    
    // Specular
    {
        float3 R = reflect(GlobalLight.Direction, normal);
        R = normalize(R);
        
        float3 cameraPosition = CameraPosition();
        float3 E = normalize(cameraPosition - worldPosition);
        
        float value = saturate(dot(R, E));
        float specular = pow(value, 10);

        specularColor = GlobalLight.Specular * Material.Specular * specular;
    }
    
    // Emissive
    
    {
        float3 cameraPosition = CameraPosition();
        float3 E = normalize(cameraPosition - worldPosition);

        float value = saturate(dot(E, normal));
        float emissive = 1.0f - value;

        emissive = smoothstep(0.0f, 1.0f, emissive);
        emissive = pow(emissive, 2);

        emissiveColor = GlobalLight.Emissive * Material.Emissive * emissive;
    }
    
    
    return ambientColor + diffuseColor + specularColor + emissiveColor;
}

float4 ComputeTeacherLight(float3 normal, float2 uv, float3 worldPosition)
{
    vector vMaterialDiffuse = DiffuseMap.Sample(LinearSampler, uv);

    
    if (vMaterialDiffuse.a < 0.3f)
        discard;
    
    // Diffuse + ambient
    vector shade = max(dot(normalize(-GlobalLight.Direction), normalize(normal)), 0.f) + 
    GlobalLight.Ambient * Material.Ambient;
    
    // Specular
    float3 reflection = reflect(normalize(GlobalLight.Direction), normalize(normal));
    float3 eyelook = worldPosition - CameraPosition();
    
    float specular = pow(max(dot(normalize(eyelook), normalize(reflection)), 0.f), 30.f);
    
    return (GlobalLight.Diffuse * vMaterialDiffuse) * saturate(shade) + (GlobalLight.Specular * Material.Specular) * specular;

}

//inout은 c++ 관점에서 레퍼런스나 포인터와 같다.
void ComputeNormalMapping(inout float3 normal, float3 tangent, float2 uv)
{
    // [0, 255] 범위를 [0, 1]의 범위로 변환
    // Sample이 0, 1로 변환해주는 작업을 해주고 있었음.
    float4 map = NormalMap.Sample(LinearSampler, uv);

    // rgb값 중 어느 하나가 값이있으면 true, 전부 없으면 false를 반환함.
    if (any(map.rgb) == false)
        return;
    
    float3 N = normalize(normal);  // z
    float3 T = normalize(tangent); // x
    float3 B = normalize(cross(N, T)); // y -> 축 2개를 알면 바이탄젠트를 구할 수 있음.
    float3x3 TBN = float3x3(T, B, N); // TS -> WS
    
    // [0, 1] 범위에서 [-1, 1] 범위로 변환
    float3 tangentSpaceNormal = (map.rgb * 2.0f - 1.0f);
    float3 worldNormal = mul(tangentSpaceNormal, TBN);
    
    normal = worldNormal;
}


#endif