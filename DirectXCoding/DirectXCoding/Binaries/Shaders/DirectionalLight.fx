/* constant buffer */
cbuffer cbPerObjectVS : register(b0) // Model constant
{
    matrix worldMatrix  : packoffset(c0);
    matrix viewMatrix   : packoffset(c4);
    matrix projMatrix   : packoffset(c8);
};

cbuffer cbPerObjectPS : register(b0)
{
    float3 Eyeposition  : packoffset(c0);
    float specExp       : packoffset(c0.w);
    float specIntensity : packoffset(c1);
};

cbuffer cbDirLightPS : register(b1)
{
    float3 ambientDown      : packoffset(c0);
    float3 ambientRange     : packoffset(c1);
    float3 dirToLight       : packoffset(c2);
    float3 dirLightColor    : packoffset(c3);
};

Texture2D DiffuseTexture    : register(t0);
SamplerState LinearSampler  : register(s0)
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = WRAP;
    AddressV = WRAP;
    AddressW = WRAP;
    MaxAnisotropy = 1;
    ComparisonFunc = ALWAYS;
    MaxLOD = 3.402823466e+38f;
};
SamplerState PointSampler   : register(s1)
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = WRAP;
    AddressV = WRAP;
    AddressW = WRAP;
    MaxAnisotropy = 1;
    ComparisonFunc = ALWAYS;
    MaxLOD = 3.402823466e+38f;
};

struct VS_INPUT
{
    float4 position : POSITION;
    float3 normal   : NORMAL;
    float2 uv       : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 position      : SV_POSITION;
    float2 uv            : TEXCOORD0;
    float3 normal        : TEXCOORD1;
    float3 worldposition : TEXCOORD2;
};


float4 DepthPrePassVS(float4 Position : POSITION) : SV_POSITION
{
    matrix matWV, matWVP;
    
    matWV = mul(worldMatrix, viewMatrix);
    matWVP = mul(matWV, projMatrix);
    
    return mul(Position, matWVP);
}

VS_OUTPUT DirectionLightVS(VS_INPUT input)
{
    VS_OUTPUT Output;
    float3 normalWorldSpace;
    
    matrix matWV, matWVP;
    
    matWV = mul(worldMatrix, viewMatrix);
    matWVP = mul(matWV, projMatrix);
    
    Output.position = mul(input.position, matWVP);
    Output.worldposition = mul(input.position, worldMatrix).xyz;
    Output.uv = input.uv;
    Output.normal = mul(input.normal, (float3x3) worldMatrix);
    
    return Output;
}


float3 CalcAmbient(float3 normal, float3 color)
{
    float up = normal.y * 0.5 * 0.5;
    
    float3 ambient = ambientDown * up * ambientRange;
    
    return ambient * color;
}

struct Material
{
    float3 normal;
    float4 diffuseColor;
    float specExp;
    float specIntensity;
};

Material PrepareMaterial(float3 normal, float2 uv)
{
    Material material;
    
    material.normal = normalize(normal);
    
    material.diffuseColor = DiffuseTexture.Sample(LinearSampler, uv);
    material.diffuseColor.rgb *= material.diffuseColor.rgb;
    
    material.specExp = specExp;
    material.specIntensity = specIntensity;
    
    return material;
}


float3 CalcDirectional(float3 position, Material material)
{
    float NDotL = dot(dirToLight, material.normal);
    float3 finalColor = dirLightColor.rgb * saturate(NDotL);
    
    // Blinn specular
    float3 ToEye = Eyeposition.xyz - position;
    ToEye = normalize(ToEye);
    float3 HalfWay = normalize(ToEye + dirToLight);
    float NDotH = saturate(dot(HalfWay, material.normal));
    finalColor += dirLightColor.rgb * pow(NDotH, material.specExp) * material.specIntensity;
    
    return finalColor * material.diffuseColor.rgb;
}

float4 DirectionalLightPS(VS_OUTPUT In) : SV_TARGET0
{
    // Prepare the material structure
    Material material = PrepareMaterial(In.normal, In.uv);
    
    // Calculate the ambient Color
    float3 finalColor = CalcAmbient(material.normal, material.diffuseColor.rgb);
    
    // Calculate the directional light
    finalColor += CalcDirectional(In.worldposition, material);
    
    return float4(finalColor, 1.0f);

}


technique11 Directionaltechnique
{
    pass DirectionalLight
    {
        VertexShader = compile vs_5_0 DirectionLightVS();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 DirectionalLightPS();
    }
}