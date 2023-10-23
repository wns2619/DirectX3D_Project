#include "LightHelper.fx"
#include "GlobalShader.fx"

MeshOut VS_MAIN(VertexAnimMesh input)
{
    MeshOut Out = (MeshOut) 0;
    
    
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
    Out.normal = mul(meshNormal, W);
    Out.worldPosition = mul(bonePosition, W);
    
    return Out;
}


PixelOut PS_MAIN(MeshOut input)
{
    PixelOut Out = (PixelOut) 0;
    
    // 조명 + 내 마테리얼로 나온 최종 색깔 값.
    Out.Color = ComputeTeacherLight(float4(input.normal, 1.f), input.uv, input.worldPosition);
    
    
    // TODO Color  * @@ 
    
    return Out;
}

technique11 MeshTechnique
{
    SOLID_PASS_VP(AnimMesh, VS_MAIN, PS_MAIN)
}