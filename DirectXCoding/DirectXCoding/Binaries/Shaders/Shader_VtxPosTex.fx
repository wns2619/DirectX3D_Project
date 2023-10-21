#include "GlobalShader.fx"
#include "LightHelper.fx"

vector NewColor = vector(1.f, 1.f, 1.f, 1.f);

VertexOut VS_MAIN(VertexTextureNormal input)
{
    VertexOut output = (VertexOut) 0;
    
    
    Matrix WVP = ComputeTransformMatrix(W, V, P);
    
    output.position = mul(float4(input.position, 1.f), WVP);
    output.uv = input.uv;
    output.normal = mul(float4(input.normal, 0.f), W);
    output.worldPosition = mul(float4(input.position, 1.f), W);
    
    return output;
}

PixelOut PS_MAIN(VertexOut input)
{
    PixelOut Out = (PixelOut) 0;
    
    vector vMaterialDiffuse = DiffuseMap.Sample(LinearSampler, input.uv) * NewColor;
    
    if (vMaterialDiffuse.a < 0.3f)
        discard;
    
    // Diffuse + ambient
    vector shade = max(dot(normalize(-GlobalLight.Direction), normalize(input.normal)), 0.f) +
    GlobalLight.Ambient * Material.Ambient;
    
    // Specular
    float3 reflection = reflect(normalize(GlobalLight.Direction), normalize(input.normal));
    float3 eyelook = input.worldPosition.xyz - CameraPosition();
    
    float specular = pow(max(dot(normalize(eyelook), normalize(reflection)), 0.f), 30.f);
    
    Out.Color = (GlobalLight.Diffuse * vMaterialDiffuse) * saturate(shade) + (GlobalLight.Specular * Material.Specular) * specular;
    
    return Out;
   
    //Out.Color = ComputeTeacherLight(input.normal, input.uv, input.worldPosition.xyz);
}

technique11 DefaultTechnique
{
    SOLID_PASS_VP(Rect, VS_MAIN, PS_MAIN)
}