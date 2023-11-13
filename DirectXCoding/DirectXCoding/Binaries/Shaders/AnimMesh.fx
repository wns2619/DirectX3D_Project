#include "LightHelper.fx"
#include "GlobalShader.fx"

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
    Out.tangent = normalize(mul(float4(input.tangent, 0.f), W));
    Out.worldPosition = mul(bonePosition, W);
    Out.vProjPos = Out.position;
    
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
    
    if (vMtrlDiffuse.a < 0.3f)
        discard;
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(input.normal.xyz * 0.5 + 0.5f, 0.f);
    Out.vDepth = vector(input.vProjPos.z / input.vProjPos.w, input.vProjPos.w / fCameraFar, 0.f, 0.f);

    
    return Out;
}

technique11 MeshTechnique
{
    SOLID_PASS_VP(AnimMesh, VS_MAIN, PS_MAIN)
}