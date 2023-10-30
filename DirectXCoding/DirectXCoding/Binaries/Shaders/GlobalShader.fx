#ifndef _GLOBAL_FX_
#define _GLOBAL_FX_

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

struct MeshOut
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


SamplerState LinearSampler : register(s0)
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

SamplerState PointSampler : register(s1)
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Wrap;
    AddressV = Wrap;
};

RasterizerState FillModeWireFrame
{
    FillMode = WireFrame;
    FrontCounterClockwise = false;
};

RasterizerState FillModeSolid
{
    CullMode = NONE;
    FillMode = SOLID;
    FrontCounterClockwise = false;
};

DepthStencilState DSS_None
{
    DepthEnable = false;
    DepthWriteMask = zero;

};

DepthStencilState DSS_Default
{
    DepthEnable = true;
    DepthWriteMask = all;
    DepthFunc = LESS_EQUAL;
};


#define SOLID_PASS_VP(name, vs, ps)                 \
pass name                                           \
{                                                   \
    SetRasterizerState(FillModeSolid);              \
    SetDepthStencilState(DSS_Default, 0);           \
    SetVertexShader(CompileShader(vs_5_0, vs()));   \
    SetPixelShader(CompileShader(ps_5_0, ps()));    \
}

#define SOLID_PASS_VPDEPTH(name, vs, ps)            \
pass name                                           \
{                                                   \
    SetRasterizerState(FillModeSolid);              \
    SetDepthStencilState(DSS_None, 0);              \
    SetVertexShader(CompileShader(vs_5_0, vs()));   \
    SetPixelShader(CompileShader(ps_5_0, ps()));    \
}

#define WIRE_PASS_VP(name, vs, ps)                  \
pass name                                           \
{                                                   \
    SetRasterizerState(FillModeWireFrame);          \
    SetDepthStencilState(DSS_Default, 0);           \
    SetVertexShader(CompileShader(vs_5_0, vs()));   \
    SetPixelShader(CompileShader(ps_5_0, ps()));    \
}

//#define NONE_PASS_VP(name, vs, ps)                  \
//pass name                                           \
//{                                                   \
//    SetRasterizerState(FillModelNone);              \
//    SetVertexShader(CompileShader(vs_5_0, vs()));   \
//    SetPixelShader(CompileShader(ps_5_0, ps()));    \
//}

float3 CameraPosition()
{
    return -V._41_42_43;
}


#endif