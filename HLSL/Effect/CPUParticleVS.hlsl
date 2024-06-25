#include "CPUParticle.hlsli"
#include "..\\3D\\Light.hlsli"

GS_IN main(VS_IN input)
{
    GS_IN output = (GS_IN) 0;

    // �ȈՓI�ȃ��C�e�B���O�v�Z
    float3 N = normalize(input.Normal);
    float3 L = normalize(-directionalLight.direction.xyz);
    float d = dot(L, N);
    float power = max(0.0f, d) * 0.5f + 0.5f; // d��0�����̏ꍇ���l������max�֐����g�p

    // output�\���̂̊e�t�B�[���h����͂���R�s�[
    output.Position = input.Position;
    output.Rotate = input.Rotate;

    // �J���[��rgb�����Ƀ��C�e�B���O�̉e����K�p
    output.Color.rgb = input.Color.rgb * power;
    output.Color.a = input.Color.a;

    output.Size = input.Size;
    output.Normal = N;
    output.Param = input.Param;

    return output;
}