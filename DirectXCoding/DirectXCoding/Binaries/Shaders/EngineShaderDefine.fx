#ifndef _ENGINEDEFINE_FX_
#define _ENGINEDEFINE_FX_


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
    CullMode = Back;
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


Texture2D DiffuseMap;
Texture2D NormalMap;
Texture2D ShadeMap;
Texture2D SpecularMap;


#endif