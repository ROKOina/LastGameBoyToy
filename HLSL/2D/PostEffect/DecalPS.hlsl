#include "FullScreenQuad.hlsli"
#include "PostEffect.hlsli"
#include "../../Common.hlsli"
#include "../../Constants.hlsli"
#include "../../noise.hlsli"
#include "../../3D/Light.hlsli"

Texture2D texturemaps : register(t0);
Texture2D positionmap : register(t1);

float4 main(VS_OUT pin) : SV_TARGET
{
    // �Ăяo���ꂽ�s�N�Z���̈ʒu��UV�l�ɕϊ�
    float2 screenuv = pin.position.xy / float2(1920, 1080);

    // PositionTarget���猻�݌Ăяo���ꂽ�s�N�Z���V�F�[�_�[�Ɠ����ꏊ�ɃA�N�Z�X���č��W�l���m�F
    float4 vViewPos = positionmap.Sample(sampler_states[POINT], screenuv);

    // Deferred�i�f�B�t�@�[�h�j�i�K�ŕ`�悳��Ă��Ȃ��ꍇ�A����^���邱�Ƃ͂ł��Ȃ��B
    if (-1.f == vViewPos.w)
    {
        discard;
    }

    // Cube�{�����[�����b�V���̃��[�J����ԂɈړ�������B
    float3 vLocal = mul(float4(vViewPos.xyz, 1.f), viewProjection).xyz;
    vLocal += 0.5f;
    if (vLocal.x < 0.f || 1.f < vLocal.x ||
    vLocal.y < 0.f || 1.f < vLocal.y ||
    vLocal.z < 0.f || 1.f < vLocal.z)
    {
        //discard;
    }

    // �{�����[�����b�V���������萬����
    float4 color = texturemaps.Sample(sampler_states[POINT], vLocal.xz);

    return color;
}