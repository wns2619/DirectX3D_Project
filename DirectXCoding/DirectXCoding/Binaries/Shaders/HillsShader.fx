#include "LightHelper.fx"
#include "GlobalShader.fx"

cbuffer LightBuffer
{
    Light Lightinfo;
}

cbuffer Material : register(b2)
{
    vector materialAmbient   = vector(0.4f, 0.4f, 0.4f, 1.f);
    vector materialSpecular  = vector(1.f, 1.f, 1.f, 1.f);
}


VertexOut VS_MAIN(VertexTextureNormal input)
{
    VertexOut output;

    matrix matWVP = ComputeTransformMatrix(W, V, P);
    
    output.position = mul(float4(input.position, 1.f), matWVP);
    output.uv = input.uv;
    output.normal = mul(float4(input.normal, 0.f), W);
    output.worldPosition = mul(float4(input.position, 1.f), W);
    
    
    return output;
}


Texture2D DiffuseTexture : register(t0);


PixelOut PS_MAIN(PixelIn In)
{
    PixelOut Out = (PixelOut) 0;
    
    vector mtldff = DiffuseTexture.Sample(LinearSampler, In.uv * 30.f);
    
    vector shader = max(dot(normalize(-Lightinfo.Direction.xyz), normalize(In.normal)), 0.f) + Lightinfo.Ambient * materialAmbient;
    
    float3 reflectDirection = reflect(normalize(Lightinfo.Direction.xyz), normalize(In.normal));
    float3 look = In.worldPosition - cameraPosition;
    
    float specular = pow(max(dot(normalize(-look), normalize(reflectDirection)), 0.f), 30.f);
    
    Out.Color = (float4(Lightinfo.Diffuse, 1.f) * mtldff) * saturate(shader) + (Lightinfo.Specular * materialSpecular) * specular;
    
    return Out;
}

technique11 DefalutTechnique
{
    SOLID_PASS_VP(HillsShader, VS_MAIN, PS_MAIN)
}