struct INSTANCE_VS_IN
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float4 tangent : TANGENT;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;

    float4 InstRotation : I_ROTATION;
    float3 InstPosition : I_POSITION;
    float3 InstScale : I_SCALE;
};

cbuffer INSTANCINGBUFFER : register(b1)
{
    row_major float4x4 global_transform;
}

//回転
float4x4 matrix_rotation_quaternion(float4 quaternion)
{
    float x = quaternion.x;
    float y = quaternion.y;
    float z = quaternion.z;
    float w = quaternion.w;

    float m00 = 1.0f - 2.0f * (y * y + z * z);
    float m01 = 2.0f * (x * y + z * w);
    float m02 = 2.0f * (x * z - y * w);

    float m10 = 2.0f * (x * y - z * w);
    float m11 = 1.0f - 2.0f * (x * x + z * z);
    float m12 = 2.0f * (y * z + x * w);

    float m20 = 2.0f * (x * z + y * w);
    float m21 = 2.0f * (y * z - x * w);
    float m22 = 1.0f - 2.0f * (x * x + y * y);

    return float4x4(m00, m01, m02, 0,
                    m10, m11, m12, 0,
                    m20, m21, m22, 0,
                    0, 0, 0, 1);
}

//スケール
float4x4 matrix_scalling(float3 scale)
{
    return float4x4(
       scale.x, 0, 0, 0,
       0, scale.y, 0, 0,
       0, 0, scale.z, 0,
       0, 0, 0, 1);
}

//移動
float4x4 matrix_transform(float3 position)
{
    return float4x4(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        position.x, position.y, position.z, 1);
}

//全て合わせたワールド座標
float4x4 calc_world_transform(float3 scale, float4 quaternion, float3 position)
{
    //行列の乗算順序はスケール→回転→移動
    return mul(matrix_transform(position), mul(matrix_rotation_quaternion(quaternion), matrix_scalling(scale)));
}