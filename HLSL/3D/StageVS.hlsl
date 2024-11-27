#include "Defalt.hlsli"
#include "../Constants.hlsli"

VS_OUT main(VS_IN vin)
{
    float3 p = float3(0, 0, 0);
    float3 n = float3(0, 0, 0);
    float3 t = float3(0, 0, 0);

    // �X�L�j���O�v�Z (�ő�4�̃{�[���E�F�C�g�ɑΉ�)
    for (int i = 0; i < 4; i++)
    {
        // �{�[���C���f�b�N�X���擾
        int boneIndex = vin.boneIndices[i];

        // �ŏI�{�[���s�� = offsetTransform * boneTransform
        float4x4 finalBoneMatrix = mul(offsetTransforms[boneIndex], boneTransforms[boneIndex]);

        // ���_�ʒu�̃X�L�j���O
        p += vin.boneWeights[i] * mul(vin.position, finalBoneMatrix).xyz;

        // �@���̃X�L�j���O�i���s�ړ������͖������邽��w = 0���g�p�j
        n += vin.boneWeights[i] * mul(float4(vin.normal, 0), finalBoneMatrix).xyz;

        // �ڐ��̃X�L�j���O
        t += vin.boneWeights[i] * mul(float4(vin.tangent, 0), finalBoneMatrix).xyz;
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