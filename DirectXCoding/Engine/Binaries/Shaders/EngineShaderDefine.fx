#ifndef _ENGINEDEFINE_FX_
#define _ENGINEDEFINE_FX_

cbuffer MatrixCBuffer : register(b0) // Model Constant
{
    matrix W : packoffset(c0);
    matrix V : packoffset(c4);
    matrix P : packoffset(c8);
};

cbuffer cbPerObjectPS : register(b0) // Model ConstantBuffer
{
    vector EyePosition : packoffset(c0);
    float specExp : packoffset(c1);
    float specIntensity : packoffset(c2);
}

struct Material
{
    vector normal;
    float4 diffuseColor;
    float specExp;
    float specIntensity;
};

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



sampler LinearSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = wrap;
    AddressV = wrap;
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

    SrcBlend = One;
    DestBlend = One;
    BlendOp = Add;
};


Texture2D DiffuseMap;
Texture2D NormalMap;
Texture2D ShadeMap;
Texture2D SpecularMap;
Texture2D DepthMap;




#endif