#include "LightHelper.fx"
#include "GlobalShader.fx"

//cbuffer Material
//{
//    vector materialAmbient = vector(0.4f, 0.4f, 0.4f, 0.4f);
//    vector materialSpecular = vector(1.f, 1.f, 1.f, 1.f);
//};

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

    //vector vMaterialdiffuse = DiffuseMap.Sample(LinearSampler, input.uv);

    //if (vMaterialdiffuse.a < 0.3f)
    //    discard;
    
    //vector shade = max(dot(normalize(-GlobalLight.Direction), normalize(input.normal)), 0.f) +
    //GlobalLight.Ambient * Material.Ambient;
    
    //float3 _reflect = float4(reflect(normalize(GlobalLight.Direction), normalize(input.normal)), 0.f);
    //float3 _look = input.worldPosition.xyz - CameraPosition();
    
    //float _specular = pow(max(dot(normalize(-_look), normalize(_reflect)), 0.f), 30.f);
    
    //Out.Color = (GlobalLight.Diffuse * vMaterialdiffuse) * saturate(shade) +
    //(GlobalLight.Specular * Material.Specular) * _specular;
    
    
   //ComputeNormalMapping(input.normal, input.tangent, input.uv);
   //Out.Color = ComputeLight(input.normal, input.uv, input.worldPosition.xyz);
    
    Out.Color = ComputeTeacherLight(input.normal, input.uv, input.worldPosition.xyz);
    
    return Out;
}

technique11 MeshTechnique
{
    SOLID_PASS_VP(AnimMesh, VS_MAIN, PS_MAIN)
}