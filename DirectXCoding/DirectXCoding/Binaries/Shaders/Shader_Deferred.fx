#include "EngineShaderDefine.fx"


cbuffer cbDirLightPS : register(b1) // Directional and Ambient light ConstantBuffers;
{
    vector ambientDown      : packoffset(c0);
    vector ambientUp        : packoffset(c1);
    vector DirToLight       : packoffset(c2);
    vector DirLightColor    : packoffset(c3);
};

cbuffer PointLightConstants : register(b2) // point Light constants
{
    vector PointLightPosition   : packoffset(c0);
    vector PointLightColor      : packoffset(c1);
    float PointLightRangeRcp    : packoffset(c2);
};

cbuffer SpotLightConstants : register(b3)
{
    vector SpotLightPos         : packoffset(c0);
    vector SpotLightColor       : packoffset(c1);
    float3 SpotDirToLight       : packoffset(c2);
    float  SpotLightRangeRcp    : packoffset(c2.w);
    float  SpotCosOuterCone     : packoffset(c3.x);
    float  SpotCosInnerConeRcp  : packoffset(c3.y);
}


struct PS_OUT_LIGHT
{
    float4 vShade : SV_TARGET0;
    float4 vSpecular : SV_TARGET1;
};

void GammaToLinear(inout vector color)
{
    color.x *= color.x;
    color.y *= color.y;
    color.z *= color.z;
}

vector CalcAmbient(float4 normal, float4 Color)
{
    float up = normal.y * 0.5 + 0.5;
    
    vector downColor = ambientDown;
    vector UpColor = ambientUp;
    vector range = vector(0.f, 0.f, 0.f, 0.f);
    
    GammaToLinear(downColor);
    GammaToLinear(UpColor);
    
    range = UpColor - downColor;
   
    float4 Ambient = downColor + up * range;
    
    return Ambient * Color;
}

// Directional light Calculation helper function
void CalcDirectional(vector position, Material material, inout PS_OUT_LIGHT finalColor)
{
    // Phong Diffuse
    vector DirColor = DirLightColor;
    GammaToLinear(DirColor);
    
    float NDotL = dot(DirToLight, material.normal);
    finalColor.vShade += float4(DirColor.rgb * saturate(NDotL), 1.f);
    
    // Blinn specular
    float3 ToEye = EyePosition.xyz - position.xyz;
    ToEye = normalize(ToEye);
    float3 HalfWay = normalize(ToEye + DirToLight.xyz);
    float NDotH = saturate(dot(HalfWay, material.normal.xyz));
    finalColor.vSpecular *= DirColor * pow(NDotH, material.specExp) * material.specIntensity;
}

void CalcPoint(vector position, Material material, inout PS_OUT_LIGHT finalColor)
{
    vector PointColor = PointLightColor;
    GammaToLinear(PointColor);
    
    float3 ToLight = PointLightPosition.xyz - position.xyz;
    float3 ToEye = EyePosition.xyz - position.xyz;
    float DistToLight = length(ToLight);
    
    vector shadeColor = vector(0.f, 0.f, 0.f, 0.f);
    vector specColor = vector(0.f, 0.f, 0.f, 0.f);
    
    // Phong Diffuse
    ToLight /= DistToLight; // normaliz
    float NDotL = saturate(dot(ToLight, material.normal.xyz));
    finalColor.vShade += float4(PointColor.rgb * NDotL, 1.f);
    
    // Blinn Specular;
    ToEye = normalize(ToEye);
    float3 HalfWay = normalize(ToEye + ToLight);
    float NDotH = saturate(dot(HalfWay, material.normal.xyz));
    finalColor.vSpecular *= PointColor * pow(NDotH, material.specExp) * material.specIntensity;
    
    // Attenuation
    float DistToLightNorm = 1.0 - saturate(DistToLight * PointLightRangeRcp);
    float Attn = DistToLightNorm * DistToLightNorm;
    
    finalColor.vShade *= Attn;
    finalColor.vSpecular *= Attn;
}

void CalcSpot(vector position, Material material, inout PS_OUT_LIGHT finalColor)
{
    vector SpotColor = SpotLightColor;
    GammaToLinear(SpotColor);

    vector shadeColor = vector(0.f, 0.f, 0.f, 1.f);
    vector specColor = vector(0.f, 0.f, 0.f, 1.f);
    
    float3 ToLight = SpotLightPos.xyz - position.xyz;
    float3 ToEye = EyePosition.xyz - position.xyz;
    float DistToLight = length(ToLight);
    
    // Phong Diffuse
    ToLight /= DistToLight; // noramlize
    float NDotL = saturate(dot(ToLight, material.normal.xyz));
    finalColor.vShade += float4(SpotColor.rgb * NDotL, 1.f);

    // Phong Specular
    ToEye = normalize(ToEye);
    float3 HalfWay = normalize(ToEye + ToLight);
    float NDotH = saturate(dot(HalfWay, material.normal.xyz));
    finalColor.vSpecular *= float4(SpotColor.rgb * pow(NDotH, material.specExp) * material.specIntensity, 1.f);
    
    // Cone attenuation
    float cosAng = dot(SpotDirToLight, ToLight);
    float conAtt = saturate((cosAng - SpotCosOuterCone) * SpotCosInnerConeRcp);
    conAtt *= conAtt;
    
    // Attenuation
    float DistToLightNorm = 1.0 - saturate(DistToLight * SpotLightRangeRcp);
    float Attn = DistToLightNorm * DistToLightNorm;
    
    finalColor.vShade *= conAtt * Attn;
    finalColor.vSpecular *= conAtt * Attn;
}


Texture2D gTexture;

cbuffer InvMatrixCBuffer
{
    Matrix projInv, viewInv;
};


cbuffer LightCBuffer
{
    vector vLightDir;
    vector vLightDiffuse;
    vector vLightAmbient;
    vector vLightSpecular;
    
   
    vector vLightPos;
    vector vLightRange;
    
    // SPOT
    float fLightRange;
    float3 vAtt;
    float Spot;
};

cbuffer MaterialCBuffer
{
    vector _vMtrlAmbient = vector(0.4f,0.4f,0.4f,1.f);
    vector _vMtrlSpecular = vector(1.f, 1.f, 1.f, 1.f);
};


struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};



VS_OUT VS_MAIN( /* 정점 */VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
	
    matrix matWV, matWVP;

    matWV = mul(W, V);
    matWVP = mul(matWV, P);

    Out.vPosition = mul(float4(In.vPosition, 1.f), matWVP);

    Out.vTexcoord = In.vTexcoord;

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};


struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};


PS_OUT PS_MAIN_DEBUG(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = gTexture.Sample(LinearSampler, In.vTexcoord);
	
    return Out;
}

PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;
    
    vector vNormalDesc = NormalMap.Sample(PointSampler, In.vTexcoord);
    vector vDepthDesc = DepthMap.Sample(PointSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y * fCameraFar;
    
    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);
    
    // PrepareMaterial
    Material material = PrepareMaterial(vNormal);
    
    // Ambient Light Color 빛의 색상
    vector AmbientColor = CalcAmbient(vNormal, DirLightColor);
    Out.vShade = AmbientColor * _vMtrlAmbient;
    
    vector vWorldPos;
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;
    
    vWorldPos = vWorldPos * fViewZ;
    vWorldPos = mul(vWorldPos, projInv);
    vWorldPos = mul(vWorldPos, viewInv);
    

    Out.vSpecular = _vMtrlSpecular;
    CalcDirectional(vWorldPos, material, Out);
    
    return Out;
}

PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;

    vector vNormalDesc = NormalMap.Sample(PointSampler, In.vTexcoord);
    vector vDepthDesc = DepthMap.Sample(PointSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y * fCameraFar;

    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);
    
    // PrepareMaterial
    Material material = PrepareMaterial(vNormal);

    // AmbientLight Gamma;
    vector AmbientColor = CalcAmbient(vNormal, PointLightColor);
    Out.vShade = AmbientColor * _vMtrlAmbient;
    
    vector vWorldPos;

    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;

    vWorldPos = vWorldPos * fViewZ;
    vWorldPos = mul(vWorldPos, projInv);
    vWorldPos = mul(vWorldPos, viewInv);

    Out.vSpecular = _vMtrlSpecular;
    CalcPoint(vWorldPos, material, Out);

    return Out;
}

PS_OUT_LIGHT PS_MAIN_SPOT(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;
    
    vector vNormalDesc = NormalMap.Sample(PointSampler, In.vTexcoord);
    vector vDepthDesc = DepthMap.Sample(PointSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y * fCameraFar;

    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

    // PrepareMaterial
    Material material = PrepareMaterial(vNormal);
    
    // AmbientLight Gamma
    vector AmbientColor = CalcAmbient(vNormal, SpotLightColor);
    Out.vShade = AmbientColor * _vMtrlAmbient;
    
    vector vWorldPos;

    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;


    vWorldPos = vWorldPos * fViewZ;
    vWorldPos = mul(vWorldPos, projInv);
    vWorldPos = mul(vWorldPos, viewInv);
    
    Out.vSpecular = _vMtrlSpecular;
    CalcSpot(vWorldPos, material, Out);
    
    return Out;
}


PS_OUT PS_MAIN_DEFERRED(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vDiffuse = DiffuseMap.Sample(PointSampler, In.vTexcoord);
    if (vDiffuse.a == 0.f)
        discard;
    
    vector vShade = ShadeMap.Sample(LinearSampler, In.vTexcoord);
    vector vSpecular = SpecularMap.Sample(LinearSampler, In.vTexcoord);
    
    
    // Vignette
     // Calculate the distance from the center of the viewport.
    float viewportRadius = 0.6f; // Adjust this as needed.
    float distance = length(In.vTexcoord - float2(0.5, 0.5)) / viewportRadius;
    
    // Calculate the vignette factor (adjust the falloff as needed).
    float vignette = 1.0 - smoothstep(0.0, 1.0, distance);
    vDiffuse *= vignette;


    Out.vColor = vDiffuse * vShade + vSpecular;
    
    
    return Out;

}

technique11 DefaultTechnique
{
    pass Target_Debug
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);
		
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
    }

    pass Light_Directional
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
    }

    pass Light_Point
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_OneBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_POINT();
    }

    pass Light_Spot
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_OneBlend, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SPOT();
    }

    pass Deferred
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEFERRED();
    }
}




