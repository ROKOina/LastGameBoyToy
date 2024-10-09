struct VS_IN
{
    float4 position : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
    float4 boneWeights : WEIGHTS;
    uint4 boneIndices : BONES;
};

#define MAX_BONES 256
cbuffer CbMesh : register(b1)
{
    row_major float4x4 boneTransforms[MAX_BONES];
};