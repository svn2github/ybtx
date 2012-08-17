///@brief 光照计算函数
///@see StaticIlluminator.cpp
///@version 1.0 忽略高光版本
///@version 1.1 增加方向光的高光. 因为顶点色的关系, 材质改为由参数传入
#define SYS_LOCAL_LIGHTS 0

/// 太阳光
void GlobalLight(in float3 worldPos,
                 in float3 worldNormal,
                 in float4 matAmbient,
                 in float4 matDiffuse,
                 in out float4 diffuse,
                 in out float4 specular)
{
    float NdotL = dot(worldNormal, -gGlobalLightDirection);
    diffuse = gGlobalAmbient * matAmbient + gGlobalLightColor * max(NdotL, 0.0f) * matDiffuse + gMatEmissive;

    float3 H = normalize(normalize(g_vEyePos - worldPos) - gGlobalLightDirection);
    float NdotH = dot(worldNormal, H);
    specular = float4(gMatSpecular.rgb, 1.0f) * gGlobalLightColor * pow(max(NdotH,0.0f), gMatSpecular.a);
}

/// 点光源
void PointLight(in int lightIndex,
                in float3 worldPos,
                in float3 worldNormal,
                in float4 matDiffuse,
                in out float4 diffuse)
{
    float3 direction = gLightPosition[lightIndex] - worldPos;
    float dist = length(direction);
    if (gLightRangeInfo[lightIndex].x < dist)
        return;
    direction = normalize(direction);
    float NdotL = dot(direction, worldNormal);
    NdotL = max(NdotL, 0.0f);
    float attenuation = 1.0f / (gLightRangeInfo[lightIndex].y +
                        gLightRangeInfo[lightIndex].z * dist +
                        gLightRangeInfo[lightIndex].w * dist * dist);
    diffuse += gLightColor[lightIndex] * matDiffuse * NdotL * attenuation;
}

/// 聚光灯
void SpotLight(in int lightIndex,
               in float3 worldPos,
               in float3 worldNormal,
               in float4 matDiffuse,
               in out float4 diffuse)
{
    float3 direction = gLightPosition[lightIndex] - worldPos;
    float dist = length(direction);
    if (gLightRangeInfo[lightIndex].x < dist)
        return;
    direction = normalize(direction);
    float NdotL = dot(direction, worldNormal);
    NdotL = max(NdotL, 0.0f);
    float attenuation = 1.0f / (gLightRangeInfo[lightIndex].y +
                        gLightRangeInfo[lightIndex].z * dist +
                        gLightRangeInfo[lightIndex].w * dist * dist);
    float cosAlpha = dot(-direction, gLightDirection[lightIndex]);
    float cosHalfPhi = cos(gLightSpotInfo[lightIndex].w * 0.5f);
    if (cosAlpha <= cosHalfPhi)
        return;
    float cosHalfTheta = cos(gLightSpotInfo[lightIndex].z * 0.5f);
    float intensity = (cosAlpha - cosHalfPhi) / (cosHalfTheta - cosHalfPhi);
    intensity = pow(intensity, gLightSpotInfo[lightIndex].y);

    diffuse += gLightColor[lightIndex] * matDiffuse * NdotL * attenuation * intensity;
}

void Lighting(in float3 worldPos,
              in float3 worldNormal,
              in float4 matAmbient,
              in float4 matDiffuse,
              in out float4 diffuse,
              in out float4 specular)
{
    GlobalLight(worldPos, worldNormal, matAmbient, matDiffuse, diffuse, specular);
#if SYS_LOCAL_LIGHTS
    for (int lightIndex = 0; lightIndex < MAX_LIGHTS; ++lightIndex)
    {
        if (lightIndex >= gLightCount)
            break;
        if (gLightSpotInfo[lightIndex].x < 0) // invalid light
            continue;
        if (gLightSpotInfo[lightIndex].x > 0)
            SpotLight(lightIndex, worldPos, worldNormal, matDiffuse, diffuse);
        else
            PointLight(lightIndex, worldPos, worldNormal, matDiffuse, diffuse);
    }
#endif
    diffuse.a = matDiffuse.a;
}
