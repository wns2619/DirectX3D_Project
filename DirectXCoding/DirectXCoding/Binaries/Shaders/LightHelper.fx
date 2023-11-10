#ifndef _LIGHTHELPER_FX_
#define _LIGHTHELPER_FX_

#include "GlobalShader.fx"


// global Buffer

//inout�� c++ �������� ���۷����� �����Ϳ� ����.
void ComputeNormalMapping(inout float3 normal, float3 tangent, float2 uv)
{
    // [0, 255] ������ [0, 1]�� ������ ��ȯ
    // Sample�� 0, 1�� ��ȯ���ִ� �۾��� ���ְ� �־���.
    float4 map = NormalMap.Sample(LinearSampler, uv);

    // rgb�� �� ��� �ϳ��� ���������� true, ���� ������ false�� ��ȯ��.
    if (any(map.rgb) == false)
        return;
    
    float3 N = normalize(normal);  // z
    float3 T = normalize(tangent); // x
    float3 B = normalize(cross(N, T)); // y -> �� 2���� �˸� ����ź��Ʈ�� ���� �� ����.
    float3x3 TBN = float3x3(T, B, N); // TS -> WS
    
    // [0, 1] �������� [-1, 1] ������ ��ȯ
    float3 tangentSpaceNormal = (map.rgb * 2.0f - 1.0f);
    float3 worldNormal = mul(tangentSpaceNormal, TBN);
    
    normal = worldNormal;
}

#endif