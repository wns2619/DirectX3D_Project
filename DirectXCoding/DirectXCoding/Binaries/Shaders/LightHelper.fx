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

struct Light
{
    float4 Position   ; // 16
    float3 Direction  ; 
    float intensity   ; // 32
    float3 Diffuse    ;
    float range       ; // 48
    float4 Ambient    ; // 64
    float4 Specular   ; // 80
    float2 spotAngles ; // 88
    int type          ; // 92
    bool enabled;
    
    float3 rotationDeg;
    bool pad;
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