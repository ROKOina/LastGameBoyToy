cbuffer Decal : register(b12)
{
    row_major float4x4 world;
    row_major float4x4 decalinverseprojection;
    float4 decalcolor;
}