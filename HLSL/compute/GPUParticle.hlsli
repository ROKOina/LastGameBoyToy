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
    row_major float4x4 world;
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
    float spiralSpeed;

    float3 orbitalVelocity;
    float spiralstrong;

    float veloRandScale;
    float speed;
    float emitStartSpeed; //  �G�t�F�N�g�̍Đ��J�n���̑��x
    float emitEndSpeed; //  �G�t�F�N�g�Đ���̍ŏI�I�ȑ��x

    float2 scale;
    int scaleVariateByLife; // �傫���̕ω��̊���Ǘ� ( TRUE : �����ɂ���ĕω� )
    int worldpos; //�p�[�e�B�N����S�������t����

    float lifeStartSize; // �p�[�e�B�N���̐������̑��x
    float lifeEndSize; // �p�[�e�B�N���̏��Ŏ��̑��x
    // -------------------  ���� �ǂ��炩�̂�  ----------------------
    float emitStartSize; // �G�t�F�N�g�̍Đ��J�n���̑��x
    float emitEndSize; // �G�t�F�N�g�Đ���̍ŏI�I�ȑ��x

    float radial;
    float buoyancy;
    float emitStartGravity;
    float emitEndGravity;

    float strechscale; //�X�g���b�`�r���{�[�h�̋��x(�L�т鎞�̑傫��)
    float3 padding;
}