#include "Defalt.hlsli"
#include "../Constants.hlsli"

VS_OUT main(VS_IN vin)
{
    float3 p = float3(0, 0, 0);
    float3 n = float3(0, 0, 0);
    float3 t = float3(0, 0, 0);

    // �X�L�j���O�v�Z (�ő�4�̃{�[���E�F�C�g�ɑΉ�)
    for (int i = 0; i < 4; ++i)
    {
        p += (vin.boneWeights[i] * mul(vin.position, boneTransforms[vin.boneIndices[i]])).xyz;
        n += (vin.boneWeights[i] * mul(float4(vin.normal.xyz, 0), boneTransforms[vin.boneIndices[i]])).xyz;
        t += (vin.boneWeights[i] * mul(float4(vin.tangent.xyz, 0), boneTransforms[vin.boneIndices[i]])).xyz;
    }

    // �o�̓f�[�^�̍쐬
    VS_OUT vout;
    vout.position = mul(float4(p, 1.0f), viewProjection); // �N���b�v��ԍ��W�֕ϊ�
    vout.world_position = p; // ���[���h���W
    vout.normal = normalize(n); // �@���𐳋K��
    vout.tangent = normalize(t); // �ڐ��𐳋K��
    vout.binormal = normalize(cross(vout.tangent, vout.normal)); // �o�C�m�[�}�����v�Z
    vout.color.rgb = vin.color.rgb * materialcolor.rgb; // ���_�F�ƃ}�e���A���F���|�����킹��
    vout.color.a = vin.color.a * materialcolor.a; // �A���t�@�l���K�p
    vout.texcoord = vin.texcoord; // �e�N�X�`�����W

    return vout;
}