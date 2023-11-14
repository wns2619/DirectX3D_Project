#ifndef _ENGINEDEFINE_FX_
#define _ENGINEDEFINE_FX_


sampler LinearSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = wrap;
    AddressV = wrap;
};

sampler LinearSamplerMirror = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Mirror;
    AddressV = Mirror;
};


sampler PointSampler = sampler_state
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = wrap;
    AddressV = wrap;
};


RasterizerState RS_Default
{
    FillMode = Solid;
    CullMode = None;
    FrontCounterClockwise = false;
};

RasterizerState RS_Sky
{
    FillMode = Solid;
    CullMode = None;
    FrontCounterClockwise = false;
};

DepthStencilState DSS_Default
{
    DepthEnable = true;
    DepthWriteMask = all;
    DepthFunc = LESS_EQUAL;
};

DepthStencilState DSS_None
{
    DepthEnable = false;
    DepthWriteMask = zero;
};

BlendState BS_Default
{
    BlendEnable[0] = false;
};

BlendState BS_AlphaBlend
{
    BlendEnable[0] = true;

    SrcBlend[0] = Src_Alpha;
    DestBlend[0] = Inv_Src_Alpha;
    BlendOp[0] = Add;

	//SrcBlend[0] = Src_Alpha;
	//DestBlend[0] = Inv_Src_Alpha;
	//BlendOp[0] = Add;

	//SrcBlend[1] = One;
	//DestBlend[1] = One;
	//BlendOp[1] = Add;
};

BlendState BS_OneBlend
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;
    BlendEnable[2] = true;

    SrcBlend = One;
    DestBlend = One;
    BlendOp = Add;
};


Texture2D<float> DepthTexture          : register(t0);
Texture2D<vector> ColorSpecIntTexture   : register(t1);
Texture2D<float3> NormalTexture         : register(t2);
Texture2D<vector> SpecPowTexture        : register(t3);

cbuffer MatrixCBuffer : register(b0) // Model Constant
{
    matrix W : packoffset(c0);
    matrix V : packoffset(c4);
    matrix P : packoffset(c8);
};


cbuffer cbPerObjectPS : register(b0) // Model ConstantBuffer
{
    vector EyePosition      : packoffset(c0);
    float specExp           : packoffset(c1);
    float specIntensity     : packoffset(c2);
}

cbuffer cbBufferUnpack : register(b0)
{
    float4      PerspectiveValues   : packoffset(c0);
    float4x4 ViewInv                : packoffset(c1);
}

cbuffer cbFog : register(b2)
{
    float3 FogColor             : packoffset(c0);
    float FogStartDepth         : packoffset(c0.w);
    float3 FogHighlightColor    : packoffset(c1);
    float FogGlobalDensity      : packoffset(c1.w);
    float3 FogSunDir            : packoffset(c2);
    float FogStartHeight        : packoffset(c2.w);
}

//#define EyePosition (ViewInv[3].xyz)

struct Material
{
    vector normal;
    float4 diffuseColor;
    float specExp;
    float specIntensity;
};

static const float2 g_SpecPowerRange = { 10.0f, 250.f };

cbuffer CameraCBuffer
{
    float fCameraFar = 50.f;
};

float3 DecodeNormal(float2 encodedNormal)
{
    vector decodedNormal = encodedNormal.xyyy * float4(2.f, 2.f, 0.f, 0.f) + float4(-1.f, -1.f, -1.f, -1.f);
    decodedNormal.z = dot(decodedNormal.xyz, -decodedNormal.xyz);
    decodedNormal.xy = sqrt(decodedNormal.z);
    
    return decodedNormal.xyz * 2.f + float3(0.f, 0.f, -1.f);
}


float ConvertZToLinearDepth(float depth)
{
    float linearDepth = PerspectiveValues.z / (depth + PerspectiveValues.w);
    return linearDepth;
}

float3 CalcWorldPos(float2 csPos, float depth)
{
    vector position;
    
    position.xy = csPos.xy * PerspectiveValues.xy * depth;
    position.z = depth;
    position.w = 1.f;
    
    return mul(position, ViewInv).xyz;
}

struct SURFACE_DATA
{
    float LinearDepth;
    float3 Color;
    float3 Normal;
    float SpecPow;
    float SpecIntensity;
};

SURFACE_DATA UnpackGBuffer(float2 UV)
{
    SURFACE_DATA Out;
    
    float depth = DepthTexture.Sample(PointSampler, UV.xy).x;
    Out.LinearDepth = ConvertZToLinearDepth(depth);
    float4 baseColorSpecInt = ColorSpecIntTexture.Sample(PointSampler, UV.xy);
    Out.Color = baseColorSpecInt.xyz;
    Out.SpecIntensity = baseColorSpecInt.w;
    Out.Normal = NormalTexture.Sample(PointSampler, UV.xy).xyz;
    Out.Normal = normalize(Out.Normal * 2.0f - 1.0f);
    Out.SpecPow = SpecPowTexture.Sample(PointSampler, UV.xy).x;
    
    return Out;
}

SURFACE_DATA UnpackGBuffer_Loc(int2 location)
{
    SURFACE_DATA Out;
    int3 location3 = int3(location, 0);
    
    float depth = DepthTexture.Load(location3).x;
    Out.LinearDepth = ConvertZToLinearDepth(depth);
    float4 baseColorSpecInt = ColorSpecIntTexture.Load(location3);
    Out.Color = baseColorSpecInt.xyz;
    Out.SpecIntensity = baseColorSpecInt.w;
    Out.Normal = NormalTexture.Load(location3).xyz;
    Out.Normal = normalize(Out.Normal * 2.0f - 1.0f);
    Out.SpecPow = SpecPowTexture.Load(location3).x;
    
    return Out;
}

void MaterialFromGBuffer(SURFACE_DATA gbd, inout Material mat)
{
    mat.normal.xyz = gbd.Normal;
    mat.diffuseColor.xyz = gbd.Color;
    mat.diffuseColor.w = 1.f; // Fully opaque
    mat.specExp = g_SpecPowerRange.x + g_SpecPowerRange.y * gbd.SpecPow;
    mat.specIntensity = gbd.SpecIntensity;
}

float4 DebugLightPS() : SV_TARGET
{
    return float4(1.f, 1.f, 1.f, 1.f);
}

Material PrepareMaterial(vector normal)
{
    Material material;
    
    // Normalize
    material.normal = normal;
    
    // Copy the specular values from the constant Buffer
    material.specExp = specExp;
    material.specIntensity = specIntensity;
    
    return material;
}

matrix ComputeTransformMatrix(matrix world, matrix view, matrix proj)
{
    matrix matWV, matWVP;
    
    matWV = mul(world, view);
    matWVP = mul(matWV, proj);
    
    return matWVP;
};



Texture2D DiffuseMap;
Texture2D NormalMap;
Texture2D ShadeMap;
Texture2D SpecularMap;
Texture2D DepthMap;




#endif