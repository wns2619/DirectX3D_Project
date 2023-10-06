#include "LightHelper.fx"
#include "GlobalShader.fx"


cbuffer LightBuffer
{
    Light Lightinfo;
};

cbuffer Material
{
    vector materialAmbient = vector(0.4f, 0.4f, 0.4f, 0.4f);
    vector materialSpecular = vector(1.f, 1.f, 1.f, 1.f);
};


struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 normal : NORMAL;
    float2 texcoord : TEXCOORD0;
    float4 worldPos : TEXCOORD1;
};

VS_OUT VS_MAIN(VertexAnimMesh input)
{
    VS_OUT Out = (VS_OUT) 0;
    
    matrix BoneMatrix = BoneMatrices[input.blendIndices.x] * input.blendWeights.x +
     BoneMatrices[input.blendIndices.y] * input.blendWeights.y +
     BoneMatrices[input.blendIndices.z] * input.blendWeights.z +
     BoneMatrices[input.blendIndices.w] * input.blendWeights.w;
    
    vector bonePosition = mul(float4(input.position, 1.f), BoneMatrix);
    
    matrix WVP = ComputeTransformMatrix(W, V, P);
    
    Out.position = mul(bonePosition, WVP);
    Out.texcoord = input.uv;
    Out.normal = mul(float4(input.normal, 0.f), W);
    Out.worldPos = mul(float4(input.position, 1.f), W);
    
    return Out;
}


struct PS_IN
{
    float4 position : SV_POSITION;
    float4 normal : NORMAL;
    float2 texcoord : TEXCOORD0;
    float4 worldPos : TEXCOORD1;
};

struct PS_OUT
{
    float4 color : SV_TARGET0;
};

Texture2D DiffuseTexture : register(t0);

PS_OUT PS_MAIN(PS_IN input)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMaterialdiffuse = DiffuseTexture.Sample(LinearSampler, input.texcoord);
    
    if (vMaterialdiffuse.a < 0.3f)
        discard;
    
    vector shade = max(dot(normalize(float4(-Lightinfo.Direction, 1.f)), normalize(input.normal)), 0.f) +
    Lightinfo.Ambient * materialAmbient;
    
    vector _reflect = reflect(normalize(float4(Lightinfo.Direction, 1.f)), normalize(input.normal));
    vector _look = input.worldPos - cameraPosition;
    
    float _specular = pow(max(dot(normalize(-_look), normalize(_reflect)), 0.f), 30.f);
    
    Out.color = (float4(Lightinfo.Diffuse, 1.f) * vMaterialdiffuse) * saturate(shade) +
    (Lightinfo.Specular * materialSpecular) * _specular;
    
    return Out;
}

technique11 MeshTechnique
{
    SOLID_PASS_VP(name, VS_MAIN, PS_MAIN)
}