cbuffer CbPostEffect : register(b3)
{
    float4 vignettecolor;
    float3 colorize;
    float brightness;
    float contrast;
    float hue;
    float saturation;
    float bloom_extraction_threshold;
    float blur_convolution_intensity;
    float exposure;
    float vignettesize;
    float vignetteintensity;
    float blurstrength;
    float blurradius;
    float blurdecay;
    float distance_to_sun;
    float3 sundirection;
    float FogScale;
    float FogAttenuationRate;
    float3 FogColor;
    float FogHeightOffset;
    float HeightFogScale;
    float HeightFogAttenuationRate;
    float HeightFogWeightRate;
};