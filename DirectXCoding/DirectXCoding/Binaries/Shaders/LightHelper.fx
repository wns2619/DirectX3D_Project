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

struct Material
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float4 Reflect;
};

matrix ComputeTransformMatrix(matrix world, matrix view, matrix proj)
{
    matrix matWV, matWVP;
    
    matWV = mul(world, view);
    matWVP = mul(matWV, proj);
    
    return matWVP;
};