#pragma once

#include <DirectXMath.h>

#include "System\Component.h"
#include "Graphics/Model/Model.h"
#include "Graphics\Model\Model.h"

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

    //Animation�N���X
public:
    //�A�j���[�V�����Đ��֐�

    //���ʂ̃A�j���[�V�����Đ��֐�
    void PlayAnimation(int animeID,bool loop,float blendSeconds);
    //�Đ�����
    bool IsPlayAnimation() const { return currentAnimation >= 0; }
    //�A�j���[�V�����X�g�b�v
    void StopAnimation();

public:
    // �A�j���[�V��������
    float GetAnimationSeconds() const { return currentSeconds; }
    void SetAnimationSeconds(float seconds) { currentSeconds = seconds; }

private:

    //�A�j���[�V�����v�Z
    void ComputeAnimation(const ModelResource::NodeKeyData& key0, const ModelResource::NodeKeyData& key1, const float rate, Model::Node&node);

private:
    //�ϐ�

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

};