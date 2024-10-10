#include "Shadow.hlsli"

[maxvertexcount(3)]
void main(triangle VS_OUT_CSM input[3], inout TriangleStream<GS_OUTPUT_CSM> output)
{
    GS_OUTPUT_CSM element;

    // �e���_���o��
    for (int i = 0; i < 3; ++i)
    {
        element.slice = input[i].slice; // �e���_�̃X���C�X��ݒ�
        element.position = input[i].position; // �e���_�̈ʒu��ݒ�
        output.Append(element); // ���_���X�g���[���ɒǉ�
    }

    output.RestartStrip(); // �X�g���b�v�̍ċN��
}