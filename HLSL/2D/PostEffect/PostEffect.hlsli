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
    float2 ssrparameter;
    float2 posteffectdummy;
};