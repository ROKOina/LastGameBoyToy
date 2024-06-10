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
    //�A�j���[�V�����؂�ւ����̌v�Z
    void ComputeSwitchAnimation(const ModelResource::NodeKeyData& key1, const float blendRate, Model::Node& node);
   
private:

    //���[�g���[�V�����֘A
    
    //���[�g���[�V�����̒l�����m�[�h������
    void SetupRootMotion(const char* rootMotionNodeName);
    //���[�g���[�V�����̍������m�[�h������
    void SetupRootMotionHip(const char* rootMotionNodeName);
    //���[�g���[�V�����X�V
    void updateRootMotion(DirectX::XMFLOAT3& translation);
    //���[�g���[�V�����̈ړ��l���v�Z
    void ComputeRootMotion();

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

    //���[�g���[�V����

    //�������m�[�h�̃C���f�b�N�X
    int rootMotionNodeIndex=-1;
    //�|�W�V����������m�[�h�̃C���f�b�N�X
    int rootMotionHipNodeIndex=-1;
    //���[�g���[�V�������邩�ǂ���
    bool rootFlag = false;
    //���[�g���[�V�����@�A�j���[�V�����X�V��������
    bool rootMotionFlag = false;
    //���[�g���[�V�����Ŏg���l��ۑ����Ă����ϐ�
    DirectX::XMFLOAT3 cahcheRootMotionTranslation = { 0,0,0 };
    //�t���[���Ԃ̍����p�ϐ�
    DirectX::XMFLOAT3 rootMotionTranslation = { 0,0,0 };




};