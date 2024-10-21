#define THREAD 1024

struct VS_OUT
{
    uint vertex_id : VERTEXID;
};

struct GS_OUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
};

struct MainParticle
{
    float3 position;
    float2 scale;
    float4 rotation;
    float3 velocity;
    float3 direction;
    float4 color;
    float lifetime;
    float age;
    int isalive;
};

//�ۑ����Ȃ����̂����Ă���
cbuffer GPUParticleConstants : register(b6)
{
    float4 rotation;
    float3 position;
    int isalive;
    float3 currentEmitVec;
    int isEmitFlg;
}

// �ۑ�������̂����Ă���
cbuffer GPUparticleSaveConstants : register(b7)
{
    float emitTime;
    float lifeTime;
    int stretchFlag;
    int isLoopFlg;

    //���F�����ꏊ�����_���A���F���a�A���F���a�̃{�����[���A���F�~�`�ɂ���p��
    float4 shape;

    float4 baseColor; // �x�[�X�ƂȂ�F

    float4 lifeStartColor; // �p�[�e�B�N���̐������̐F
    float4 lifeEndColor; // �p�[�e�B�N���̏��Ŏ��̐F
    // -------------------  ���� �ǂ��炩�̂�  ----------------------
    float4 emitStartColor; // �G�t�F�N�g�̍Đ��J�n���̐F
    float4 emitEndColor; // �G�t�F�N�g�Đ���̍ŏI�I�ȐF

    int colorVariateByLife; // �F�̕ω��̊���Ǘ� ( TRUE : �����ɂ���ĕω� )
    float3 colorScale; // �F���X�ɖ��邭����ȂǂŎg�p

    float3 emitVec;
    float padding3;

    float3 orbitalVelocity;
    float padding4;

    float veloRandScale;
    float speed;
    float emitStartSpeed; //  �G�t�F�N�g�̍Đ��J�n���̑��x
    float emitEndSpeed; //  �G�t�F�N�g�Đ���̍ŏI�I�ȑ��x

    float2 scale;
    int scaleVariateByLife; // �傫���̕ω��̊���Ǘ� ( TRUE : �����ɂ���ĕω� )
    float padding6; // �傫���̕ω��̊���Ǘ� ( TRUE : �����ɂ���ĕω� )

    float lifeStartSize; // �p�[�e�B�N���̐������̑��x
    float lifeEndSize; // �p�[�e�B�N���̏��Ŏ��̑��x
    // -------------------  ���� �ǂ��炩�̂�  ----------------------
    float emitStartSize; // �G�t�F�N�g�̍Đ��J�n���̑��x
    float emitEndSize; // �G�t�F�N�g�Đ���̍ŏI�I�ȑ��x

    float radial;
    float buoyancy;
    float emitStartGravity;
    float emitEndGravity;
}

//�����_���֐�
float rand(float2 co) //�����̓V�[�h�l�ƌĂ΂��@�����l��n���Γ������̂�Ԃ�
{
    return frac(sin(dot(co.xy, float2(12.9898, 78.233))) * 43758.5453);
}

// 1�����̃����_���Ȓl���Z�o����
float random(float n)
{
    return frac(sin(n) * 43758.5453123);
}

uint XOrShift32(uint value)
{
    value = value ^ (value << 13);
    value = value ^ (value >> 17);
    value = value ^ (value << 5);
    return value;
}

float random(float2 seed)
{
    uint h = XOrShift32(asuint(seed.x));
    h = XOrShift32(h ^ asuint(seed.y));
    return asfloat((h & 0x007FFFFF) | 0x40000000) - 3.0;
}