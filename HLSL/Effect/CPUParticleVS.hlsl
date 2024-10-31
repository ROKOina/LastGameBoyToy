#include "CPUParticle.hlsli"
#include "..\\3D\\Light.hlsli"

GS_IN main(VS_IN input)
{
    GS_IN output = (GS_IN) 0;

    // �@���ƌ������̐��K��
    float3 N = normalize(input.Normal);
    float3 L = normalize(-directionalLight.direction.xyz);

    // �f�B�t���[�Y�v�Z
    float d = dot(L, N);
    float diffusePower = max(0.0f, d) * 0.5f + 0.5f;

    // output�\���̂̊e�t�B�[���h����͂���R�s�[
    output.Position = input.Position;
    output.Rotate = input.Rotate;

    // �J���[��rgb�����Ƀ��C�e�B���O�̉e����K�p
    output.Color.rgb = input.Color.rgb * diffusePower;
    output.Color.a = input.Color.a;

    output.Size = input.Size;
    output.Normal = N;
    output.Param = input.Param;

    return output;
}