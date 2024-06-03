cbuffer CbPostEffect : register(b3)
{
    float3 colorize;
    float brightness;
    float contrast;
    float hue;
    float saturation;
    float bloom_extraction_threshold;
    float blur_convolution_intensity;
    float exposure;
    float2 Cbdummy;
};