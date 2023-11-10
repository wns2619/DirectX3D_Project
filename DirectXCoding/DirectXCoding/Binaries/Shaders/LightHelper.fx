#ifndef _LIGHTHELPER_FX_
#define _LIGHTHELPER_FX_

#include "GlobalShader.fx"


// global Buffer

//inout은 c++ 관점에서 레퍼런스나 포인터와 같다.
void ComputeNormalMapping(inout float3 normal, float3 tangent, float2 uv)
{
    // [0, 255] 범위를 [0, 1]의 범위로 변환
    // Sample이 0, 1로 변환해주는 작업을 해주고 있었음.
    float4 map = NormalMap.Sample(LinearSampler, uv);

    // rgb값 중 어느 하나가 값이있으면 true, 전부 없으면 false를 반환함.
    if (any(map.rgb) == false)
        return;
    
    float3 N = normalize(normal);  // z
    float3 T = normalize(tangent); // x
    float3 B = normalize(cross(N, T)); // y -> 축 2개를 알면 바이탄젠트를 구할 수 있음.
    float3x3 TBN = float3x3(T, B, N); // TS -> WS
    
    // [0, 1] 범위에서 [-1, 1] 범위로 변환
    float3 tangentSpaceNormal = (map.rgb * 2.0f - 1.0f);
    float3 worldNormal = mul(tangentSpaceNormal, TBN);
    
    normal = worldNormal;
}

#endif