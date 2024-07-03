struct VS_OUT_CSM
{
    float4 position : POSITION;
    uint slice : SLICE;
};

struct GS_OUTPUT_CSM
{
    float4 position : SV_POSITION;
    uint slice : SV_RENDERTARGETARRAYINDEX;
};

cbuffer ShadowConstants : register(b4)
{
    row_major float4x4 lightviewprojection[4];
    float4 cascadedplanedistances;
}

cbuffer ShadowParameter : register(b9)
{
    float shadowdepthbias;
    float shadowcolor;
    float shadowfilterradius;
    uint shadowsamplecount;
}