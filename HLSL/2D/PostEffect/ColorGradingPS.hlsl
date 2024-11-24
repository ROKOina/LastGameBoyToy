#include "FullScreenQuad.hlsli"
#include "PostEffect.hlsli"
#include "../../Common.hlsli"
#include "../../Constants.hlsli"
#include "../../noise.hlsli"
#include "../../3D/Light.hlsli"

Texture2D texturemaps : register(t0);
Texture2D depth_map : register(t1);
Texture2D outlinecolor : register(t2);

//���邳(��ʑS�̂�)�ƃR���g���X�g(�摜�̖���)
float3 brightness_contrast(float3 fragment_color, float brightness, float contrast)
{
    fragment_color += brightness;
    if (contrast > 0.0)
    {
        fragment_color = (fragment_color - 0.5) / (1.0 - contrast) + 0.5;
    }
    else if (contrast < 0.0)
    {
        fragment_color = (fragment_color - 0.5) * (1.0 + contrast) + 0.5;
    }
    return fragment_color;
}

//�F���ƍʓx
float3 hue_saturation(float3 fragment_color, float hue, float saturation)
{
    float angle = hue * 3.14159265;
    float s = sin(angle), c = cos(angle);
    float3 weights = (float3(2.0 * c, -sqrt(3.0) * s - c, sqrt(3.0) * s - c) + 1.0) / 3.0;
    fragment_color = float3(dot(fragment_color, weights.xyz), dot(fragment_color, weights.zxy), dot(fragment_color, weights.yzx));
    float average = (fragment_color.r + fragment_color.g + fragment_color.b) / 3.0;
    if (saturation > 0.0)
    {
        fragment_color += (average - fragment_color) * (1.0 - 1.0 / (1.001 - saturation));
    }
    else
    {
        fragment_color += (average - fragment_color) * (-saturation);
    }
    return fragment_color;
}

// Vignette�̌v�Z
float vignette(float2 uv)
{
    // ���S����̋������v�Z
    float2 distance = uv - float2(0.5, 0.5);
    float len = length(distance);

    // �����Ɋ�Â��ăr�l�b�g���ʂ��v�Z
    float vignette = smoothstep(vignettesize, vignettesize - vignetteintensity, len);
    return vignette;
}

// �G�b�W���o (Sobel �t�B���^)
float ComputeSobelEdge(float2 texcoord, float width, float height, Texture2D depth_map, SamplerState sampler_states)
{
    float2 uvDist = float2(1.0 / width, 1.0 / height);
    float centerDepth = depth_map.Sample(sampler_states, texcoord); // ���S�̐[�x
    float4 depthDiag;
    float4 depthAxis;

    // �Ίp�����̐[�x
    depthDiag.x = depth_map.Sample(sampler_states, texcoord + uvDist); // �E��
    depthDiag.y = depth_map.Sample(sampler_states, texcoord + uvDist * float2(-1.0f, 1.0f)); // ����
    depthDiag.z = depth_map.Sample(sampler_states, texcoord - uvDist * float2(-1.0f, 1.0f)); // �E��
    depthDiag.w = depth_map.Sample(sampler_states, texcoord - uvDist); // ����

    // �������̐[�x
    depthAxis.x = depth_map.Sample(sampler_states, texcoord + uvDist * float2(0.0f, 1.0f)); // ��
    depthAxis.y = depth_map.Sample(sampler_states, texcoord - uvDist * float2(1.0f, 0.0f)); // ��
    depthAxis.z = depth_map.Sample(sampler_states, texcoord + uvDist * float2(1.0f, 0.0f)); // �E
    depthAxis.w = depth_map.Sample(sampler_states, texcoord - uvDist * float2(0.0f, 1.0f)); // ��

    // Sobel�W��
    const float4 vertDiagCoeff = float4(-1.0f, -1.0f, 1.0f, 1.0f); // �Ίp (�㉺���E)
    const float4 horizDiagCoeff = float4(1.0f, -1.0f, 1.0f, -1.0f);
    const float4 vertAxisCoeff = float4(-2.0f, 0.0f, 0.0f, 2.0f); // ������ (�㉺)
    const float4 horizAxisCoeff = float4(0.0f, -2.0f, 2.0f, 0.0f); // ������ (���E)

    // Sobel �̐��������Ɛ��������v�Z
    float4 sobelH = depthDiag * horizDiagCoeff + depthAxis * horizAxisCoeff;
    float4 sobelV = depthDiag * vertDiagCoeff + depthAxis * vertAxisCoeff;
    float sobelX = dot(sobelH, float4(1.0f, 1.0f, 1.0f, 1.0f));
    float sobelY = dot(sobelV, float4(1.0f, 1.0f, 1.0f, 1.0f));

    // Sobel �G�b�W���x (���z�̑傫��)
    return sqrt(sobelX * sobelX + sobelY * sobelY);
}

// �r���[��ԍ��W�����[���h��ԍ��W�ɕϊ�
float4 ConvertToWorldPosition(float2 texcoord, float centerDepth, float width, float height, float4x4 inverseviewprojection)
{
    float3 viewPos;
    viewPos.xy = (texcoord * 2.0f - 1.0f) * float2(width, height) * centerDepth;
    viewPos.z = centerDepth;

    float4 clipPos = float4(viewPos, 1.0f);
    float4 worldPos = mul(clipPos, inverseviewprojection);
    return worldPos / worldPos.w;
}

// �����Ɋ�Â����A�E�g���C���������l�̌v�Z
float ComputeOutlineThreshold(float distance, float minThreshold, float maxThreshold)
{
    // ���`��Ԃŋ����ɉ������������l���v�Z
    return lerp(maxThreshold, minThreshold, saturate(distance / 10.0f));
}

// �֊s���`��̏����`�F�b�N
float ComputeDepthEdge(float sobel, float distance, float minDistance, float threshold)
{
    bool drawOutline = distance > minDistance;
    return (drawOutline && sobel > threshold) ? 1.0f : 0.0f;
}

// ���C�������֐�: �A�E�g���C���`��
float4 OutlineEffect(float2 texcoord, float width, float height, float4x4 inverseviewprojection, float3 cameraposition)
{
    // Sobel �G�b�W���o
    float sobel = ComputeSobelEdge(texcoord, width, height, depth_map, sampler_states[LINEAR]);

    // �[�x���̎擾
    float centerDepth = depth_map.Sample(sampler_states[LINEAR], texcoord);

    // ���[���h���W�ƃJ���������v�Z
    float4 worldPos = ConvertToWorldPosition(texcoord, centerDepth, width, height, inverseviewprojection);
    float distance = length(worldPos.xyz - cameraposition);

    // �����ɉ�����臒l�̌v�Z
    float minThreshold = 0.001; // �ŏ��������l
    float maxThreshold = 0.01; // �ő債�����l
    float threshold = ComputeOutlineThreshold(distance, minThreshold, maxThreshold);

    // �G�b�W���o�Ɨ֊s���`��
    float minDistance = 1.0; // �J�����ɋ߂�����ꍇ�͕`�悵�Ȃ�
    float depthEdge = ComputeDepthEdge(sobel, distance, minDistance, threshold);

    // �ŏI�I�ȃA�E�g���C���̐F
    return float4(depthEdge, depthEdge, depthEdge, 1.0f) * outlinecolor.Sample(sampler_states[LINEAR], texcoord);
}

//���W�A���u���[
float3 radialblur(float2 texcoord)
{
    const int sample = 16;
    float2 centerposition = texcoord - float2(0.5f, 0.5f);
    float distance = length(centerposition);
    float factor = blurstrength / float(sample) * distance;
    factor *= smoothstep(blurradius, blurradius * blurdecay, distance);

    float3 color = float3(0.0f, 0.0f, 0.0f);
    for (int i = 0; i < sample; ++i)
    {
        float sampleoffset = 1.0f - factor * i;
        color += texturemaps.Sample(sampler_states[LINEAR], float2(0.5f, 0.5f) + (centerposition * sampleoffset)).rgb;
    }

    color /= float(sample); // �F�̕��ς����
    return color;
}

float4 main(VS_OUT pin) : SV_TARGET
{
    //�w�肳�ꂽ�~�b�v�}�b�v���x���̃e�N�X�`���}�b�v�̎��������擾���A�A�X�y�N�g����v�Z
    uint2 dimensions;
    uint mip_level = 0, number_of_samples;
    texturemaps.GetDimensions(mip_level, dimensions.x, dimensions.y, number_of_samples);

    //�e�N�X�`���}�b�v�̕��ƍ����̔䗦
    const float aspect = (float) dimensions.y / dimensions.x;

    //�[�x�}�b�v����s�N�Z���̐[�x�����T���v�����O
    float scene_depth = depth_map.Sample(sampler_states[BLACK_BORDER_LINEAR], pin.texcoord).r;

    //�[�x�l�� NDC��ԂŎg����(NDC���W�ɕϊ����Ă���)
    float4 ndc_position;
    ndc_position.x = pin.texcoord.x * +2 - 1;
    ndc_position.y = pin.texcoord.y * -2 + 1;
    ndc_position.z = scene_depth;
    ndc_position.w = 1;

    //�iNDC�j�����[���h���W�ɕϊ�
    float4 world_position = mul(ndc_position, inverseviewprojection);
    world_position = world_position / world_position.w;

    // �V�[���̃e�N�X�`���}�b�v���T���v�����O
    float4 sampled_color = texturemaps.Sample(sampler_states[POINT], pin.texcoord.xy);

    //���W�A���u���[
    sampled_color.rgb = lerp(sampled_color.rgb, radialblur(pin.texcoord).rgb, blurstrength);

    // �֊s�����ʂ̕`�� (�֊s�����G�b�W�����ɂ̂ݓK�p)
    sampled_color.rgb += OutlineEffect(pin.texcoord.xy, dimensions.x, dimensions.y, inverseviewprojection, cameraposition);

    // �r�l�b�g���ʂ̓K�p
    sampled_color.rgb = lerp(sampled_color.rgb * vignettecolor.rgb, sampled_color.rgb, vignette(pin.texcoord.xy));

    // ���邳�ƃR���g���X�g�̒���
    sampled_color.rgb = brightness_contrast(sampled_color.rgb, brightness, contrast);

    // �F���ƍʓx�̒���
    sampled_color.rgb = hue_saturation(sampled_color.rgb, hue, saturation);

    // �ŏI�I�ȐF��Ԃ�
    return sampled_color;
}