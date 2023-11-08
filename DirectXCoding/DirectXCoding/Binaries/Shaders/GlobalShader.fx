#ifndef _GLOBAL_FX_
#define _GLOBAL_FX_

#include "EngineShaderDefine.fx"

cbuffer TransformBuffer
{
    matrix W;
};

cbuffer GlobalBuffer
{
    matrix V;
    matrix P;
};

cbuffer BoneMatricesBuffer
{
    matrix BoneMatrices[256];
};

struct Vertex
{
    float3 position : POSITION;
};

struct VertexTexture
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
};

struct VertexColor
{
    float3 position : POSITION;
    float4 Color : COLOR;
};

struct VertexTextureNormal
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

struct VertexMesh
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float3 tangent : TANGENT;
    float3 bittangent : BITTANGENT;
};

// Anim
struct VertexAnimMesh
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float3 tangent : TANGENT;
    float3 bittangent : BITTANGENT;
    
    uint4 blendIndices : BLENDINDEX;
    float4 blendWeights : BLENDWEIGHT;
};

struct VertexOut
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
    float4 worldPosition : TEXCOORD1;
};

struct VertexAnimMeshOut
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float3 tangent : TANGENT;
    float3 bittangent : BITTANGENT;
    
    uint4 blendIndices : BLENDINDEX;
    float4 blendWeights : BLENDWEIGHT;
    
    float4 worldPosition : TEXCOORD1;
};

struct PixelIn
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
    float4 worldPosition : TEXCOORD1;
};

struct PixelOut
{
    float4 Color : SV_TARGET0;
};


#define SOLID_PASS_VP(name, vs, ps)                                    \
pass name                                                              \
{                                                                      \
    SetRasterizerState(RS_Default);                                    \
    SetDepthStencilState(DSS_Default, 0);                              \
	SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff); \
    SetVertexShader(CompileShader(vs_5_0, vs()));                      \
    SetPixelShader(CompileShader(ps_5_0, ps()));                       \
}

float3 CameraPosition()
{
    return -V._41_42_43;
}


#endif