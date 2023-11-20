#include "LightHelper.fx"
#include "GlobalShader.fx"

cbuffer deltatime
{
    float _time = 0.f;
}

Texture2D NoiseMap;

VertexAnimMeshOut VS_MAIN(VertexAnimMesh input)
{
    VertexAnimMeshOut Out = (VertexAnimMeshOut) 0;
    
    
    float fW = 1.f - (input.blendWeights.x + input.blendWeights.y + input.blendWeights.z);
    
    matrix BoneMatrix = BoneMatrices[input.blendIndices.x] * input.blendWeights.x +
     BoneMatrices[input.blendIndices.y] * input.blendWeights.y +
     BoneMatrices[input.blendIndices.z] * input.blendWeights.z +
     BoneMatrices[input.blendIndices.w] * fW;
    
    vector bonePosition = mul(float4(input.position, 1.f), BoneMatrix);
    vector meshNormal = mul(float4(input.normal, 0.f), BoneMatrix);
    
    
    matrix WVP = ComputeTransformMatrix(W, V, P);
    
   
    Out.position = mul(bonePosition, WVP);
    Out.uv = input.uv;
    Out.normal = normalize(mul(meshNormal, W));
    Out.worldPosition = mul(bonePosition, W);
    Out.vProjPos = Out.position;
    Out.tangent = normalize(mul(float4(input.tangent, 0.f), W));
    Out.bittangent = normalize(cross(Out.normal.xyz, Out.tangent));
    
    return Out;
}

struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
};

PS_OUT PS_MAIN(VertexAnimMeshOut input)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = DiffuseMap.Sample(LinearSampler, input.uv);
    vector vDissolve = NoiseMap.Sample(LinearSampler, input.uv * 3.f);
    vector vNormalDesc = NormalMap.Sample(LinearSampler, input.uv);
    float3 vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
    float3x3 WorldMatrix = float3x3(input.tangent, input.bittangent, input.normal.xyz);
    vNormal = normalize(mul(vNormal, WorldMatrix));
    
    float dissolveDuration = 5.f;
 

    if(vMtrlDiffuse.a < 0.3f)
        discard;
    
   
    float outlineThreshold = 0.5f;
    float outlineWidth = 0.3f;
    float edgeStart = outlineThreshold;
    float edgeEnd = outlineThreshold + outlineWidth;
    
    float fDissolveAlpha = saturate(_time > 0 ? 1.f - _time / dissolveDuration + vDissolve.r : 1.f);
    
    if (fDissolveAlpha < 0.3f)
        discard;
 
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(vNormal.xyz * 0.5 + 0.5f, 0.f);
    Out.vDepth = vector(input.vProjPos.z / input.vProjPos.w, input.vProjPos.w / fCameraFar, 0.f, 0.f);

    
    return Out;
}

technique11 MeshTechnique
{
    SOLID_PASS_VP(AnimMesh, VS_MAIN, PS_MAIN)
}