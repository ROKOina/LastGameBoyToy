#pragma once

#include <DirectXMath.h>

#include "System\Component.h"
#include "Graphics/Model/Model.h"
#include "Graphics\Model\Model.h"
#include "Graphics/Model/ModelResource.h"

//�A�j���[�V����
class AnimationCom : public Component
{
    // �R���|�[�l���g�I�[�o�[���C�h
public:
    AnimationCom() {}
    ~AnimationCom() {}

    // ���O�擾
    const char* GetName() const override { return "Animation"; }

    // �J�n����
    void Start() override;

    // �X�V����
    void Update(float elapsedTime) override;

    // GUI�`��
    void OnGUI() override;

    // �A�j���[�V�����X�V
    void AnimationUpdata(float elapsedTime);

    //�㔼�g�ʃA�j���[�V�����X�V
    void AnimationUpperUpdate(float elapsedTime);
    //�����g�ʃA�j���[�V�����X�V
    void AnimationLowerUpdate(float elapsedTime);
    //Animation�N���X

public:
    //�A�j���[�V�����X�V�^�C�v
    enum  AnimationType
    {
        NormalAnimation,
        UpperLowerAnimation,
        UpperBlendLowerAnimation,
    };

public:
    //�A�j���[�V�����Đ��֐�

    //���ʂ̃A�j���[�V�����Đ��֐�
    void PlayAnimation(int animeID, bool loop, bool rootFlag = false, float blendSeconds = 0.25f);
    //�㔼�g�����A�j���[�V�����Đ�
    void PlayUpperBodyOnlyAnimation(int upperAnimaId, bool loop, float blendSeconds);
    //�����g�����A�j���[�V�����Đ�
    void PlayLowerBodyOnlyAnimation(int lowerAnimaId, bool loop, bool rootFlag, float blendSeconds);
    //�Đ�����
    bool IsPlayAnimation() const { return currentAnimation >= 0; }
    //�㔼�g�A�j���[�V�����Đ������H
    bool IsPlayUpperAnimation();
    //�����g�A�j���[�V�����Đ������H
    bool IsPlayLowerAnimation();

    //�A�j���[�V�����X�g�b�v
    void StopAnimation();

    //�A�j��ID��Ԃ�
    int FindAnimation(const char* animeName);

public:
    //�Q�b�^�[�Z�b�^�[

    // �A�j���[�V��������
    float GetAnimationSeconds() const { return currentSeconds; }
    void SetAnimationSeconds(float seconds) { currentSeconds = seconds; }

    // ���݂̃A�j���[�V�����ԍ��擾
    int GetCurrentAnimationIndex()const { return currentAnimation; }
    int GetCurrentUpperAnimationIndex()const { return currentUpperAnimation; }
    int GetCurrentLowerAnimationIndex()const { return currentLowerAnimation; }

    //���[�g���[�V�����̒l�����m�[�h������
    void SetupRootMotion(const char* rootMotionNodeName);
    //���[�g���[�V�����̍������m�[�h������
    void SetupRootMotionHip(const char* rootMotionNodeName);
    //���[�g���[�V�����X�V
    void updateRootMotion(DirectX::XMFLOAT3& translation);
    //�A�j���[�V�����X�V�؂�ւ�
    void SetUpAnimationUpdate(int updateId);
private:

    //�A�j���[�V�����v�Z
    void ComputeAnimation(const ModelResource::NodeKeyData& key0, const ModelResource::NodeKeyData& key1, const float rate, Model::Node& node);
    //�A�j���[�V�����؂�ւ����̌v�Z
    void ComputeSwitchAnimation(const ModelResource::NodeKeyData& key1, const float blendRate, Model::Node& node);

    //AimIK�֐�
    void AimIK();

    //�m�[�h��T��
    void SearchAimNode();

    //�㔼�g�Ɖ����g�̃m�[�h�𕪂���
    void SeparateNode();

private:

    //���[�g���[�V�����֘A

    //���[�g���[�V�����̈ړ��l���v�Z
    void ComputeRootMotion();

private:

    //Debug�p�I���A�j���[�V�������擾
    ModelResource::Animation* GetSelectionAnimation();

private:
    //�ϐ�

    //�m�[�}���A�j���[�V����

    //���݂̃A�j���[�V����ID
    int								currentAnimation = -1;
    //�A�j���[�V�����̌��݂̎���
    float							currentSeconds = 0.0f;
    //�A�j���[�V������ύX�������̕ϐ�
    float                           animationChangeRate = 0.0f;
    //�A�j���[�V������ύX���̌��݂̎���
    float                           animationChangeTime = 0.0f;
    //�A�j���[�V���������[�v�����邩�H
    bool							loopAnimation = false;
    //�A�j���[�V�������I�������邩�H
    bool							endAnimation = false;

    //���[�g���[�V����

    //�������m�[�h�̃C���f�b�N�X
    int rootMotionNodeIndex = -1;
    //�|�W�V����������m�[�h�̃C���f�b�N�X
    int rootMotionHipNodeIndex = -1;
    //���[�g���[�V�������邩�ǂ���
    bool                            rootFlag = false;
    //���[�g���[�V�����@�A�j���[�V�����X�V��������
    bool                            rootMotionFlag = false;
    //���[�g���[�V�����Ŏg���l��ۑ����Ă����ϐ�
    DirectX::XMFLOAT3               cahcheRootMotionTranslation = { 0,0,0 };
    //�t���[���Ԃ̍����p�ϐ�
    DirectX::XMFLOAT3               rootMotionTranslation = { 0,0,0 };

    //�㔼�g�����g�֘A

    //���݂̍Đ����Ă���㔼�g�����g�A�j���[�V�����ԍ�
    int                             currentUpperAnimation = -1;
    int                             currentLowerAnimation = -1;
    //�㔼�g�����g�̃A�j���[�V�����ԍ�
    int                             upperAnimationIndex = 0;
    int                             lowerAnimationIndex = 0;
    //�㔼�g�̃m�[�h�̔ԍ�
    int                             upperID = 2;
    //�����g�m�[�h�̔ԍ�
    int                             lowerID = 1;
    //�㔼�g�����g�A�j���[�V������ύX�������̕ϐ�
    float                           upperAnimationChangeRate = 0.0f;
    float                           lowerAnimationChangeRate = 0.0f;
    //�㔼�g�����g�A�j���[�V������ύX���̌��݂̎���
    float                           upperAnimationChangeTime = 0.0f;
    float                           lowerAnimationChangeTime = 0.0f;
    //�㔼�g�����g�̃A�j���[�V�����̒���
    float                           upperAnimationSecondsLength = 0.0f;
    float                           lowerAnimationSecondsLength = 0.0f;
    //�㔼�g�����g�A�j���[�V�����̌��݂̎���
    float                           upperCurrentAnimationSeconds = 0.0f;
    float                           lowerCurrentAnimationSeconds = 0.0f;
    //�㔼�g�����g�A�j���[�V�����u�����h�̃u�����h���[�g
    float                           upperBlendRate = 0.0f;
    float                           lowerBlendRate = 0.0f;
    //�㔼�g�����g�A�j���[�V�����̃u�����h���[�g
    float                           upperRate = 0.0f;
    float                           lowerRate = 0.0f;

    //�㔼�g�����g�̃A�j���[�V�����Đ������H
    bool                            upperAnimaPlay = false;
    bool                            lowerAnimaPlay = false;
    //�㔼�g�����g�̃A�j���[�V�����Đ����� �Đ���=true �Đ����Ă��Ȃ�=false
    bool                            upperIsPlayAnimation = false;
    bool                            lowerIsPlayAnimation = false;
    //�㔼�g�����g�A�j���[�V�������I�����Ă��邩
    bool                            animationUpperEndFlag = false;
    bool                            animationLowerEndFlag = false;
    //�㔼�g�����g�A�j���[�V���������[�v���Ă��邩
    bool                            animationUpperLoopFlag = false;
    bool                            animationLowerLoopFlag = false;
    //�A�j���[�V�����̏I���1�t���[���O�ɗ��t���O
    bool                            beforeOneFream = false;
    //�㔼�g�����g�I�������̃t���O �I����true
    bool                            upperAnimationEndFlag = false;
    bool                            lowerAnimationEndFlag = false;
    //�㔼�g�����g�⊮�t���O
    bool                            upperComplementFlag = false;
    bool                            lowerComplementFlag = false;

    //�A�j���[�V�����X�V�ϐ�
    int animaType = 0;

    //AimIK�p�ϐ�
    std::vector<int>AimBone;

    //�㔼�g�A�j���[�V����
    std::vector<Model::Node*> upperNodes;
    //�����g�A�j���[�V����
    std::vector<Model::Node*> lowerNodes;
};