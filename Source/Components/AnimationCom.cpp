#include "AnimationCom.h"

#include "RendererCom.h"
#include "TransformCom.h"
#include "CameraCom.h"
#include "../GameSource/Math/Mathf.h"
#include "Character/CharacterCom.h"
#include <imgui.h>
#include <cassert>

// �J�n����
void AnimationCom::Start()
{
    //�m�[�h����
    SearchAimNode();
}

// �X�V����
void AnimationCom::Update(float elapsedTime)
{
    switch (animaType)
    {
        //���ʂ̃A�j���[�V�����X�V
    case AnimationType::NormalAnimation:
        //�X�V����
        AnimationUpdata(elapsedTime);
        break;
        //�㔼�g�ʃA�j���[�V�����X�V
    case AnimationType::UpperLowerAnimation:
        AnimationUpperUpdate(elapsedTime);
        AnimationLowerUpdate(elapsedTime);
        break;
        //�㔼�g�ʏ㔼�g�A�j���[�V�����u�����h�X�V
    case AnimationType::UpperBlendLowerAnimation:
        break;
    }
}

// GUI�`��
bool isAnimLoop;
void AnimationCom::OnGUI()
{
    //if (!GetGameObject()->GetComponent<RendererCom>())return;

    const ModelResource* resource = GetGameObject()->GetComponent<RendererCom>()->GetModel()->GetResource();
    const std::vector<ModelResource::Animation>& animations = resource->GetAnimations();
    //���f�����烊�\�[�X���擾
    Model* model = GetGameObject()->GetComponent<RendererCom>()->GetModel();

    ImGui::Checkbox("animationLoop", &isAnimLoop);
    ImGui::Separator();
    int index = 0;
    for (ModelResource::Animation anim : animations)
    {
        index++;
    }

    int animationIndex = 0;
    ModelResource::Animation* selectionAnimation = GetSelectionAnimation();
    std::vector<ModelResource::Animation>& animations1 = const_cast<std::vector<ModelResource::Animation>&>(model->GetResource()->GetAnimations());
    std::vector<ModelResource::Animation>::iterator it = animations1.begin();
    while (it != animations1.end())
    {
        ModelResource::Animation& animation = (*it);

        ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_Leaf;
        if (selectionAnimation == &animation)
        {
            nodeFlags |= ImGuiTreeNodeFlags_Selected;
        }

        ImGui::SetNextItemWidth(58);
        ImGui::DragFloat(animation.name.c_str(), &animation.animationspeed);

        ImGui::TreeNodeEx(&animation, nodeFlags, animation.name.c_str());

        // �N���b�N����ƑI��
        if (ImGui::IsItemClicked())
        {
            currentAnimation = animationIndex;
            PlayAnimation(currentAnimation, isAnimLoop, false, 0.2f);
        }

        ImGui::TreePop();
        ++animationIndex;
        ++it;
    }
}

//Debug�p�I���A�j���[�V�������擾
ModelResource::Animation* AnimationCom::GetSelectionAnimation()
{
    //���f�����烊�\�[�X���擾
    Model* model = GetGameObject()->GetComponent<RendererCom>()->GetModel();

    const std::vector<ModelResource::Animation>& animations = model->GetResource()->GetAnimations();
    if (currentAnimation >= 0 && currentAnimation < static_cast<int>(animations.size()))
    {
        return const_cast<ModelResource::Animation*>(&animations.at(currentAnimation));
    }
    return nullptr;
}

//�A�j���[�V�����X�V
void AnimationCom::AnimationUpdata(float elapsedTime)
{
    //���f�����烊�\�[�X���擾
    Model* model = GetGameObject()->GetComponent<RendererCom>()->GetModel();
    const ModelResource* resource = model->GetResource();

    //�u�����h���̌v�Z
    float blendRate = 1.0f;
    if (animationChangeTime > 0)
    {
        animationChangeRate += animationChangeTime;
        blendRate = animationChangeRate;
        if (blendRate > 1.0f)blendRate = 1.0f;
    }

    if (currentAnimation < 0)
    {
        return;
    }

    if (resource->GetAnimations().empty())
    {
        return;
    }

    const ModelResource::Animation& animation = resource->GetAnimations().at(currentAnimation);

    const std::vector<ModelResource::Keyframe>& keyframes = animation.keyframes;
    int keyCount = static_cast<int>(keyframes.size());
    for (int keyIndex = 0; keyIndex < keyCount - 1; ++keyIndex)
    {
        // ���݂̎��Ԃ��ǂ̃L�[�t���[���̊Ԃɂ��邩���肷��
        const ModelResource::Keyframe& keyframe0 = keyframes.at(keyIndex);
        const ModelResource::Keyframe& keyframe1 = keyframes.at(keyIndex + 1);

        if (currentSeconds >= keyframe0.seconds && currentSeconds <= keyframe1.seconds)
        {
            float rate = (currentSeconds - keyframe0.seconds) / (keyframe1.seconds - keyframe0.seconds);
            assert(model->GetNodes().size() == keyframe0.nodeKeys.size());
            assert(model->GetNodes().size() == keyframe1.nodeKeys.size());
            int nodeCount = static_cast<int>(model->GetNodes().size());
            for (int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
            {
                // �Q�̃L�[�t���[���Ԃ̕⊮�v�Z
                const ModelResource::NodeKeyData& key0 = keyframe0.nodeKeys.at(nodeIndex);
                const ModelResource::NodeKeyData& key1 = keyframe1.nodeKeys.at(nodeIndex);

                if (blendRate < 1.0f)
                {
                    //�A�j���[�V�����؂�ւ����̌v�Z
                    ComputeSwitchAnimation(key1, blendRate, model->GetNodes()[nodeIndex]);
                }
                else
                {
                    //�A�j���[�V�����v�Z
                    ComputeAnimation(key0, key1, rate, model->GetNodes()[nodeIndex]);
                }
            }

            //AimIK�v�Z
            AimIK();

            break;
        }
    }

    // �ŏI�t���[������
    if (endAnimation)
    {
        endAnimation = false;
        currentAnimation = -1;
        return;
    }

    // ���Ԍo��
    currentSeconds += elapsedTime * animation.animationspeed;
    if (currentSeconds >= animation.secondsLength)
    {
        if (loopAnimation)
        {
            //�Đ����Ԃ������߂�
            currentSeconds -= animation.secondsLength;
            //���[�g���[�V�����p�̌v�Z
            if (rootMotionNodeIndex >= 0)
            {
                //��N���A�����Hips���̒l�������̂ŃA�j���[�V�����̍ŏ��̃t���[����Hips�̒l�����ď�����
                cahcheRootMotionTranslation.x = animation.keyframes[0].nodeKeys[rootMotionHipNodeIndex].translate.x;
                cahcheRootMotionTranslation.z = animation.keyframes[0].nodeKeys[rootMotionHipNodeIndex].translate.z;

                //�L���b�V�����[�g���[�V�����ŃN���A
                model->GetNodes()[rootMotionHipNodeIndex].translate.x = cahcheRootMotionTranslation.x;
                model->GetNodes()[rootMotionHipNodeIndex].translate.z = cahcheRootMotionTranslation.z;
            }
        }
        else
        {
            currentSeconds = animation.secondsLength;
            endAnimation = true;
        }
    }

    rootMotionFlag = true;
    if (rootFlag)
    {
        //���[�g���[�V�����v�Z
        ComputeRootMotion();
    }
}

//�㔼�g�ʃA�j���[�V�����X�V
void AnimationCom::AnimationUpperUpdate(float elapsedTime)
{
    //���f�����烊�\�[�X���擾
    Model* model = GetGameObject()->GetComponent<RendererCom>()->GetModel();

    //�Đ����ł��Ȃ��Ȃ珈�����Ȃ�
    if (!IsPlayUpperAnimation())return;

    //�u�����h���̌v�Z
    float blendRate = 1.0f;
    if (upperAnimationChangeTime > 0)
    {
        upperAnimationChangeRate += upperAnimationChangeTime;
        blendRate = upperAnimationChangeRate;
        if (blendRate > 1.0f)blendRate = 1.0f;
    }

    //�w��̃A�j���[�V�����f�[�^���擾
    const std::vector<ModelResource::Animation>& animations = model->GetResource()->GetAnimations();
    const ModelResource::Animation& animation = animations.at(currentUpperAnimation);

    //�A�j���[�V�����f�[�^����L�[�t���[���f�[�^���X�g���擾
    const std::vector<ModelResource::Keyframe>& Keyframes = animation.keyframes;
    int keyCount = static_cast<int>(Keyframes.size());
    for (int keyIndex = 0; keyIndex < keyCount - 1; ++keyIndex)
    {
        //���݂̎��Ԃ��ǂ̃L�[�t���[���̊Ԃɂ��邩���肷��
        const ModelResource::Keyframe& keyframe0 = Keyframes.at(keyIndex);
        const ModelResource::Keyframe& keyframe1 = Keyframes.at(keyIndex + 1);
        if (upperCurrentAnimationSeconds >= keyframe0.seconds && upperCurrentAnimationSeconds < keyframe1.seconds)
        {
            //�Đ����ԂƃL�[�t���[���̎��Ԃ���⊮�����Z�o����
            float rate = (upperCurrentAnimationSeconds - keyframe0.seconds) / (keyframe1.seconds - keyframe0.seconds);

            int nodeCount = static_cast<int>(model->GetNodes().size());
            for (int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
            {
                //2�̃L�[�t���[���Ԃ̕⊮�v�Z
                const ModelResource::NodeKeyData& key0 = keyframe0.nodeKeys.at(nodeIndex);
                const ModelResource::NodeKeyData& key1 = keyframe1.nodeKeys.at(nodeIndex);

                if (blendRate < 1.0f)
                {
                    if (model->GetNodes()[nodeIndex].layer == 1 && upperIsPlayAnimation)
                    {
                        //���݂̎p���Ǝ��̃L�[�t���[���Ƃ̎p���̕⊮
                        ComputeSwitchAnimation(key1, blendRate, model->GetNodes()[nodeIndex]);
                    }
                }
                //�ʏ�̌v�Z
                else
                {
                    if (model->GetNodes()[nodeIndex].layer == 1 && upperIsPlayAnimation)
                    {
                        ComputeAnimation(key0, key1, rate, model->GetNodes()[nodeIndex]);
                    }
                }
            }

            AimIK();

            break;
        }
    }

    //���Ԍo��
    upperCurrentAnimationSeconds += elapsedTime * animation.animationspeed;

    //�ŏI�t���[��
    if (animationUpperEndFlag)
    {
        animationUpperEndFlag = false;
        currentUpperAnimation = -1;
        upperIsPlayAnimation = false;
        return;
    }

    //�Đ����Ԃ��I�[���Ԃ𒴂�����
    if (upperCurrentAnimationSeconds >= animation.secondsLength && animationUpperLoopFlag == false)
    {
        animationUpperEndFlag = true;
        upperComplementFlag = true;
    }

    //�Đ����Ԃ��I�[���Ԃ𒴂�����
    if (upperCurrentAnimationSeconds >= animation.secondsLength)
    {
        //�Đ����Ԃ������߂�
        upperCurrentAnimationSeconds -= animation.secondsLength;
    }
}

//�����g�ʃA�j���[�V�����X�V
void AnimationCom::AnimationLowerUpdate(float elapsedTime)
{
    //���f�����烊�\�[�X���擾
    Model* model = GetGameObject()->GetComponent<RendererCom>()->GetModel();

    //�Đ����ł��Ȃ��Ȃ珈�����Ȃ�
    if (!IsPlayLowerAnimation())return;

    //�u�����h���̌v�Z
    float blendRate = 1.0f;
    if (lowerAnimationChangeTime > 0)
    {
        lowerAnimationChangeRate += lowerAnimationChangeTime;
        blendRate = lowerAnimationChangeRate;
        if (blendRate > 1.0f)blendRate = 1.0f;
    }

    //�w��̃A�j���[�V�����f�[�^���擾
    const std::vector<ModelResource::Animation>& animations = model->GetResource()->GetAnimations();
    const ModelResource::Animation& animation = animations.at(currentLowerAnimation);
    const ModelResource::Animation& animationTwo = animations.at(lowerAnimationTwoIndex);
    const ModelResource::Animation& animationThree = animations.at(lowerAnimationThreeIndex);
    const ModelResource::Animation& animationFour = animations.at(lowerAnimationFourIndex);

    //�A�j���[�V�����f�[�^����L�[�t���[���f�[�^���X�g���擾
    const std::vector<ModelResource::Keyframe>& Keyframes = animation.keyframes;
    const std::vector<ModelResource::Keyframe>& TwoKeyframes = animationTwo.keyframes;
    const std::vector<ModelResource::Keyframe>& ThreeKeyframes = animationThree.keyframes;
    const std::vector<ModelResource::Keyframe>& FourKeyframes = animationFour.keyframes;
    int keyCount = static_cast<int>(Keyframes.size());
    for (int keyIndex = 0; keyIndex < keyCount - 1; ++keyIndex)
    {
        //���݂̎��Ԃ��ǂ̃L�[�t���[���̊Ԃɂ��邩���肷��
        const ModelResource::Keyframe& keyframe0 = Keyframes.at(keyIndex);
        const ModelResource::Keyframe& keyframe1 = Keyframes.at(keyIndex + 1);
        const ModelResource::Keyframe& walkFront = Keyframes.at(keyIndex + 1);
        const ModelResource::Keyframe& walkBack = TwoKeyframes.at(keyIndex+1);
        const ModelResource::Keyframe& walkRight = ThreeKeyframes.at(keyIndex+1);
        const ModelResource::Keyframe& walkLeft = FourKeyframes.at(keyIndex+1);

        if (lowerCurrentAnimationSeconds >= keyframe0.seconds && lowerCurrentAnimationSeconds < keyframe1.seconds)
        {
            //�Đ����ԂƃL�[�t���[���̎��Ԃ���⊮�����Z�o����
            float rate = (lowerCurrentAnimationSeconds - keyframe0.seconds) / (keyframe1.seconds - keyframe0.seconds);

            int nodeCount = static_cast<int>(model->GetNodes().size());
            for (int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
            {
                //2�̃L�[�t���[���Ԃ̕⊮�v�Z
               
               

                if (lowerBlendType == 0)
                {
                    const ModelResource::NodeKeyData& key0 = keyframe0.nodeKeys.at(nodeIndex);
                    const ModelResource::NodeKeyData& key1 = keyframe1.nodeKeys.at(nodeIndex);

                    if (blendRate < 1.0f)
                    {
                        if (model->GetNodes()[nodeIndex].layer == 2 && lowerIsPlayAnimation)
                        {
                            //���݂̎p���Ǝ��̃L�[�t���[���Ƃ̎p���̕⊮
                            ComputeSwitchAnimation(key1, blendRate, model->GetNodes()[nodeIndex]);
                        }
                    }
                    //�ʏ�̌v�Z
                    else
                    {
                        if (model->GetNodes()[nodeIndex].layer == 2 && lowerIsPlayAnimation)
                        {

                            ComputeAnimation(key0, key1, rate, model->GetNodes()[nodeIndex]);

                        }
                    }
                }
                else if (lowerBlendType == 1)
                {
                    const ModelResource::NodeKeyData& key0 = keyframe0.nodeKeys.at(nodeIndex);
                    const ModelResource::NodeKeyData& key1 = walkFront.nodeKeys.at(nodeIndex);

                    if (blendRate < 1.0f)
                    {
                        if (model->GetNodes()[nodeIndex].layer == 2 && lowerIsPlayAnimation)
                        {
                            //���݂̎p���Ǝ��̃L�[�t���[���Ƃ̎p���̕⊮
                            ComputeSwitchAnimation(key1, blendRate, model->GetNodes()[nodeIndex]);
                        }
                    }
                    //�ʏ�̌v�Z
                    else
                    {
                        if (model->GetNodes()[nodeIndex].layer == 2 && lowerIsPlayAnimation)
                        {

                            ComputeAnimation(key0, key1, lowerRate, model->GetNodes()[nodeIndex]);

                        }
                    }
                }
                else if (lowerBlendType == 2)
                {
                    float walkBlendRate = 0.0f;

                    float stickAngle = GetGameObject()->GetComponent<CharacterCom>()->GetStickAngle();

                    //�E�����
                    if (stickAngle >= 0.0f && stickAngle < 90.0f)
                    {
                        walkBlendRate = stickAngle / 90.0f;

                        const ModelResource::NodeKeyData& key0 = walkRight.nodeKeys.at(nodeIndex);
                        const ModelResource::NodeKeyData& key1 = walkFront.nodeKeys.at(nodeIndex);

                        if (blendRate < 1.0f)
                        {
                            if (model->GetNodes()[nodeIndex].layer == 2 && lowerIsPlayAnimation)
                            {
                                //�O��̃A�j���[�V�����Ƃ̃u�����h
                                ComputeWalkIdleAnimation(key0, key1, blendRate, walkBlendRate, model->GetNodes()[nodeIndex]);
                            }
                        }
                        else
                        {
                            if (model->GetNodes()[nodeIndex].layer == 2 && lowerIsPlayAnimation)
                            {

                                ComputeAnimation(key0, key1, walkBlendRate, model->GetNodes()[nodeIndex]);

                            }
                        }

                    }
                    else if (stickAngle >= 90.0f && stickAngle < 180.0f)
                    {
                        const ModelResource::NodeKeyData& key0 = walkFront.nodeKeys.at(nodeIndex);
                        const ModelResource::NodeKeyData& key1 = walkLeft.nodeKeys.at(nodeIndex);


                        walkBlendRate = (stickAngle - 90.0f) / 90.0f;

                        if (blendRate < 1.0f)
                        {
                            if (model->GetNodes()[nodeIndex].layer == 2 && lowerIsPlayAnimation)
                            {
                                //�O��̃A�j���[�V�����Ƃ̃u�����h
                                ComputeWalkIdleAnimation(key0, key1, blendRate, walkBlendRate, model->GetNodes()[nodeIndex]);
                            }
                        }
                        else
                        {
                            if (model->GetNodes()[nodeIndex].layer == 2 && lowerIsPlayAnimation)
                            {

                                ComputeAnimation(key0, key1, walkBlendRate, model->GetNodes()[nodeIndex]);

                            }
                        }

                    }
                    else if (stickAngle >= 180.0f && stickAngle < 270.0f)
                    {
                        const ModelResource::NodeKeyData& key0 = walkLeft.nodeKeys.at(nodeIndex);
                        const ModelResource::NodeKeyData& key1 = walkBack.nodeKeys.at(nodeIndex);


                        walkBlendRate = (stickAngle - 180.0f) / 90.0f;

                        if (blendRate < 1.0f)
                        {
                            if (model->GetNodes()[nodeIndex].layer == 2 && lowerIsPlayAnimation)
                            {
                                //�O��̃A�j���[�V�����Ƃ̃u�����h
                                ComputeWalkIdleAnimation(key0, key1, blendRate, walkBlendRate, model->GetNodes()[nodeIndex]);
                            }
                        }
                        else
                        {
                            if (model->GetNodes()[nodeIndex].layer == 2 && lowerIsPlayAnimation)
                            {

                                ComputeAnimation(key0, key1, walkBlendRate, model->GetNodes()[nodeIndex]);

                            }
                        }
                    }
                    else if (stickAngle >= 270.0f && stickAngle < 360.0f)
                    {
                        const ModelResource::NodeKeyData& key0 = walkBack.nodeKeys.at(nodeIndex);
                        const ModelResource::NodeKeyData& key1 = walkRight.nodeKeys.at(nodeIndex);


                        walkBlendRate = (stickAngle - 270.0f) / 90.0f;

                        if (blendRate < 1.0f)
                        {
                            if (model->GetNodes()[nodeIndex].layer == 2 && lowerIsPlayAnimation)
                            {
                                //�O��̃A�j���[�V�����Ƃ̃u�����h
                                ComputeWalkIdleAnimation(key0, key1, blendRate, walkBlendRate, model->GetNodes()[nodeIndex]);
                            }
                        }
                        else
                        {
                            if (model->GetNodes()[nodeIndex].layer == 2 && lowerIsPlayAnimation)
                            {

                                ComputeAnimation(key0, key1, walkBlendRate, model->GetNodes()[nodeIndex]);

                            }
                        }
                    }
                }
            }
            break;
        }
    }

    //���Ԍo��
    lowerCurrentAnimationSeconds += elapsedTime * animation.animationspeed;

    //�ŏI�t���[��
    if (animationLowerEndFlag)
    {
        animationLowerEndFlag = false;
        currentLowerAnimation = -1;
        lowerIsPlayAnimation = false;
        return;
    }

    //�Đ����Ԃ��I�[���Ԃ𒴂�����
    if (lowerCurrentAnimationSeconds >= animation.secondsLength)
    {
        if (animationLowerLoopFlag)
        {
            //�Đ����Ԃ������߂�
            lowerCurrentAnimationSeconds -= animation.secondsLength;
            //���[�g���[�V�����p�̌v�Z
            if (rootMotionNodeIndex >= 0)
            {
                //��N���A�����Hips���̒l�������̂ŃA�j���[�V�����̍ŏ��̃t���[����Hips�̒l�����ď�����
                cahcheRootMotionTranslation.x = animation.keyframes[0].nodeKeys[rootMotionHipNodeIndex].translate.x;
                cahcheRootMotionTranslation.z = animation.keyframes[0].nodeKeys[rootMotionHipNodeIndex].translate.z;

                //�L���b�V�����[�g���[�V�����ŃN���A
                model->GetNodes()[rootMotionHipNodeIndex].translate.x = cahcheRootMotionTranslation.x;
                model->GetNodes()[rootMotionHipNodeIndex].translate.z = cahcheRootMotionTranslation.z;
            }
        }
        else
        {
            animationLowerEndFlag = true;
            lowerComplementFlag = true;
        }
    }

    rootMotionFlag = true;
    if (rootFlag)
    {
        //���[�g���[�V�����v�Z
        ComputeRootMotion();
    }
}

//�㔼�g�A�j���[�V�����Đ������H
bool AnimationCom::IsPlayUpperAnimation()
{
    //���f�����烊�\�[�X���擾
    Model* model = GetGameObject()->GetComponent<RendererCom>()->GetModel();
    if (currentUpperAnimation < 0)return false;
    if (currentUpperAnimation >= model->GetResource()->GetAnimations().size())return false;
    return true;
}

//�����g�A�j���[�V�����Đ������H
bool AnimationCom::IsPlayLowerAnimation()
{
    //���f�����烊�\�[�X���擾
    Model* model = GetGameObject()->GetComponent<RendererCom>()->GetModel();
    if (currentLowerAnimation < 0)return false;
    if (currentLowerAnimation >= model->GetResource()->GetAnimations().size())return false;
    return true;
}

//���ʂ̃A�j���[�V�����Đ��֐�
void AnimationCom::PlayAnimation(int animeID, bool loop, bool rootFlag, float blendSeconds)
{
    currentAnimation = animeID;
    loopAnimation = loop;
    endAnimation = false;
    this->rootFlag = rootFlag;
    currentSeconds = 0.0f;
    animationChangeTime = blendSeconds;
    animationChangeRate = 0.0f;
}

//�㔼�g�̂݃A�j���[�V�����Đ��֐�
void AnimationCom::PlayUpperBodyOnlyAnimation(int upperAnimaId, bool loop, float blendSeconds)
{
    currentUpperAnimation = upperAnimaId;
    upperCurrentAnimationSeconds = 0.0f;
    animationUpperLoopFlag = loop;
    animationUpperEndFlag = false;
    upperAnimationChangeTime = blendSeconds;
    upperIsPlayAnimation = true;
    beforeOneFream = false;
    upperAnimationChangeRate = 0.0f;
}

//�����g�̂݃A�j���[�V�����Đ��֐�
void AnimationCom::PlayLowerBodyOnlyAnimation(int lowerAnimaId,int lowerAnimeTwoId,int lowerAnimeThreeId,int lowerAnimeFourId, bool loop, bool rootFlga,int blendType, float animeChangeRate,float animeBlendRate)
{
    currentLowerAnimation = lowerAnimaId; //�O
    lowerAnimationTwoIndex = lowerAnimeTwoId;//��
    lowerAnimationThreeIndex = lowerAnimeThreeId;//�E
    lowerAnimationFourIndex = lowerAnimeFourId;//��
    lowerCurrentAnimationSeconds = 0.0f;
    animationLowerLoopFlag = loop;
    animationLowerEndFlag = false;
    lowerAnimationChangeTime = animeChangeRate;
    lowerIsPlayAnimation = true;
    beforeOneFream = false;
    lowerAnimationChangeRate = 0.0f;
    this->rootFlag = rootFlga;
    lowerBlendType = blendType;

}

//�A�j���[�V�����X�g�b�v
void AnimationCom::StopAnimation()
{
    currentAnimation = -1;
}

int AnimationCom::FindAnimation(const char* animeName)
{
    //���f�����烊�\�[�X���擾
    Model* model = GetGameObject()->GetComponent<RendererCom>()->GetModel();
    const ModelResource* resource = model->GetResource();

    for (int i = 0; i < resource->GetAnimations().size(); i++) {
        if (animeName == resource->GetAnimations().at(i).name) {
            return i;
        }
    }

    assert(!"�����̖��O�̃A�j���[�V�����́A�u�Ȃɂ���������!!�v");

    return -1;
}

//�A�j���[�V�����X�V�؂�ւ�
void AnimationCom::SetUpAnimationUpdate(int updateId)
{
    animaType = updateId;
}

//�A�j���[�V�����v�Z
void AnimationCom::ComputeAnimation(const ModelResource::NodeKeyData& key0, const ModelResource::NodeKeyData& key1, const float rate, Model::Node& node)
{
    DirectX::XMVECTOR S0 = DirectX::XMLoadFloat3(&key0.scale);
    DirectX::XMVECTOR S1 = DirectX::XMLoadFloat3(&key1.scale);
    DirectX::XMVECTOR R0 = DirectX::XMLoadFloat4(&key0.rotate);
    DirectX::XMVECTOR R1 = DirectX::XMLoadFloat4(&key1.rotate);
    DirectX::XMVECTOR T0 = DirectX::XMLoadFloat3(&key0.translate);
    DirectX::XMVECTOR T1 = DirectX::XMLoadFloat3(&key1.translate);

    DirectX::XMVECTOR S = DirectX::XMVectorLerp(S0, S1, rate);
    DirectX::XMVECTOR R = DirectX::XMQuaternionSlerp(R0, R1, rate);
    DirectX::XMVECTOR T = DirectX::XMVectorLerp(T0, T1, rate);

    DirectX::XMStoreFloat3(&node.scale, S);
    DirectX::XMStoreFloat4(&node.rotate, R);
    DirectX::XMStoreFloat3(&node.translate, T);
}

//�A�j���[�V�����؂�ւ����̌v�Z
void AnimationCom::ComputeSwitchAnimation(const ModelResource::NodeKeyData& key1, const float blendRate, Model::Node& node)
{
    DirectX::XMVECTOR S1 = DirectX::XMLoadFloat3(&key1.scale);
    DirectX::XMVECTOR R1 = DirectX::XMLoadFloat4(&key1.rotate);
    DirectX::XMVECTOR T1 = DirectX::XMLoadFloat3(&key1.translate);
    DirectX::XMVECTOR MS1 = DirectX::XMLoadFloat3(&node.scale);
    DirectX::XMVECTOR MR1 = DirectX::XMLoadFloat4(&node.rotate);
    DirectX::XMVECTOR MT1 = DirectX::XMLoadFloat3(&node.translate);

    DirectX::XMVECTOR S = DirectX::XMVectorLerp(MS1, S1, blendRate);
    DirectX::XMVECTOR R = DirectX::XMQuaternionSlerp(MR1, R1, blendRate);
    DirectX::XMVECTOR T = DirectX::XMVectorLerp(MT1, T1, blendRate);

    DirectX::XMStoreFloat3(&node.scale, S);
    DirectX::XMStoreFloat4(&node.rotate, R);
    DirectX::XMStoreFloat3(&node.translate, T);
}


void AnimationCom::ComputeWalkIdleAnimation(const ModelResource::NodeKeyData& key0, const ModelResource::NodeKeyData& key1, float blendRate, float walkRate, Model::Node& node)
{
    DirectX::XMVECTOR S0 = DirectX::XMLoadFloat3(&key0.scale);
    DirectX::XMVECTOR S1 = DirectX::XMLoadFloat3(&key1.scale);
    DirectX::XMVECTOR R0 = DirectX::XMLoadFloat4(&key0.rotate);
    DirectX::XMVECTOR R1 = DirectX::XMLoadFloat4(&key1.rotate);
    DirectX::XMVECTOR T0 = DirectX::XMLoadFloat3(&key0.translate);
    DirectX::XMVECTOR T1 = DirectX::XMLoadFloat3(&key1.translate);
    DirectX::XMVECTOR NS = DirectX::XMLoadFloat3(&node.scale);
    DirectX::XMVECTOR NR = DirectX::XMLoadFloat4(&node.rotate);
    DirectX::XMVECTOR NT = DirectX::XMLoadFloat3(&node.translate);
   

    DirectX::XMVECTOR S = DirectX::XMVectorLerp(S0, S1, walkRate);
    DirectX::XMVECTOR R = DirectX::XMQuaternionSlerp(R0, R1, walkRate);
    DirectX::XMVECTOR T = DirectX::XMVectorLerp(T0, T1, walkRate);

    S = DirectX::XMVectorLerp(NS, S, blendRate);
    R = DirectX::XMQuaternionSlerp(NR, R, blendRate);
    T = DirectX::XMVectorLerp(NT, T, blendRate);

    DirectX::XMStoreFloat3(&node.scale, S);
    DirectX::XMStoreFloat4(&node.rotate, R);
    DirectX::XMStoreFloat3(&node.translate, T);
    
}

//AimIK�֐�
void AnimationCom::AimIK()
{
    // �Q�[���I�u�W�F�N�g�̃����_���[�R���|�[�l���g���烂�f�����擾
    Model* model = GetGameObject()->GetComponent<RendererCom>()->GetModel();

    // FPS�J�����̑O�������̃��[���h��Ԃł̃^�[�Q�b�g�ʒu���擾
    if (!GameObjectManager::Instance().Find("cameraPostPlayer"))
    {
        return;
    }
    DirectX::XMFLOAT3 target = GameObjectManager::Instance().Find("cameraPostPlayer")->GetComponent<CameraCom>()->GetFront();
    DirectX::XMVECTOR targetVec = DirectX::XMLoadFloat3(&target);

    // �v���C���[�̃��[���h�g�����X�t�H�[���̋t�s����擾
    DirectX::XMMATRIX playerTransformInv = DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&GetGameObject()->transform_->GetWorldTransform()));

    for (size_t neckBoneIndex : AimBone)
    {
        // ���f������G�C���{�[���m�[�h���擾
        Model::Node& aimbone = model->GetNodes()[neckBoneIndex];

        // �G�C���{�[���̃��[���h��Ԃł̈ʒu���擾
        DirectX::XMFLOAT3 aimPosition = { aimbone.worldTransform._41, aimbone.worldTransform._42, aimbone.worldTransform._43 };

        // �^�[�Q�b�g�ʒu���v���C���[�̃��[�J����Ԃɕϊ�
        DirectX::XMStoreFloat3(&target, DirectX::XMVector4Transform(targetVec, playerTransformInv));

        // �G�C���{�[������^�[�Q�b�g�ւ̃��[�J����Ԃł̃x�N�g�����v�Z
        DirectX::XMFLOAT3 toTarget = { target.x - aimPosition.x, target.y - aimPosition.y, target.z - aimPosition.z };
        DirectX::XMVECTOR toTargetVec = DirectX::XMLoadFloat3(&toTarget);

        // ���[�J����Ԃł̃A�b�v�x�N�g�����`
        DirectX::XMFLOAT3 up = { 0, 0, 1 };
        DirectX::XMVECTOR upVec = DirectX::XMLoadFloat3(&up);

        // �G�C���{�[���̃O���[�o���g�����X�t�H�[���̋t�s����擾
        DirectX::XMMATRIX inverseGlobalTransform = DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&aimbone.worldTransform));

        // toTarget��up�x�N�g�����G�C���{�[���̃��[�J����Ԃɕϊ�
        toTargetVec = DirectX::XMVector3TransformNormal(toTargetVec, inverseGlobalTransform);
        upVec = DirectX::XMVector3TransformNormal(upVec, inverseGlobalTransform);

        // ��]����up�x�N�g����toTarget�x�N�g���̊O�ςƂ��Čv�Z
        DirectX::XMVECTOR axis = DirectX::XMVector3Cross(upVec, toTargetVec);

        // up�x�N�g����toTarget�x�N�g���̊Ԃ̉�]�p���v�Z
        float angle = DirectX::XMVectorGetX(DirectX::XMVector3AngleBetweenVectors(upVec, toTargetVec));

        // ��]�p�𐧌�
        angle = (std::min)(angle, DirectX::XMConvertToRadians(50.0f));

        // �J�����̌����ɂ���ĉ�]�������C��
        DirectX::XMVECTOR cameraForward = DirectX::XMLoadFloat3(&target); // �����ŃJ�����̑O���x�N�g�����g�p
        if (DirectX::XMVectorGetX(DirectX::XMVector3Dot(cameraForward, targetVec)) < 0)
        {
            angle = -angle;
        }

        // �v�Z�������Ɗp�x�ŉ�]�s����쐬
        DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationAxis(DirectX::XMVector3Normalize(axis), angle);

        // ���݂̉�]�ƖڕW��]���擾
        DirectX::XMVECTOR currentQuat = DirectX::XMLoadFloat4(&aimbone.rotate);
        DirectX::XMVECTOR targetQuat = DirectX::XMQuaternionRotationMatrix(rotation);

        // ���`�⊮�̌W����ݒ�i0.0����1.0�̊ԁj
        float lerpFactor = 0.9f; // �⊮����ݒ�

        // �N�H�[�^�j�I���̐��`�⊮
        DirectX::XMVECTOR interpolatedQuat = DirectX::XMQuaternionSlerp(currentQuat, targetQuat, lerpFactor);

        // �v�Z������]���G�C���{�[���ɓK�p
        DirectX::XMStoreFloat4(&aimbone.rotate, interpolatedQuat);
    }
}

//�{�[����T��
void AnimationCom::SearchAimNode()
{
    Model* model = GetGameObject()->GetComponent<RendererCom>()->GetModel();

    for (size_t nodeIndex = 0; nodeIndex < model->GetNodes().size(); ++nodeIndex)
    {
        const Model::Node& node = model->GetNodes().at(nodeIndex);

        if (strstr(node.name, "Spine") == node.name)
        {
            AimBone.push_back(static_cast<int>(nodeIndex));
        }
    }
}

//���[�g���[�V�����̒l�����m�[�h������
void AnimationCom::SetupRootMotion(const char* rootMotionNodeIndex)
{
    Model* model = GetGameObject()->GetComponent<RendererCom>()->GetModel();
    this->rootMotionNodeIndex = model->FindNodeIndex(rootMotionNodeIndex);
}

//���[�g���[�V�����̍������m�[�h������
void AnimationCom::SetupRootMotionHip(const char* rootMotionNodeName)
{
    Model* model = GetGameObject()->GetComponent<RendererCom>()->GetModel();
    this->rootMotionHipNodeIndex = model->FindNodeIndex(rootMotionNodeName);
}

//���[�g���[�V�����̈ړ��l���v�Z
void AnimationCom::ComputeRootMotion()
{
    Model* model = GetGameObject()->GetComponent<RendererCom>()->GetModel();

    if (!rootMotionFlag)
    {
        return;
    }
    if (rootMotionNodeIndex < 0)
    {
        return;
    }

    //�O�̃t���[���ƍ���̃t���[���̈ړ��ʃf�[�^�̍����ʂ����߂�
    rootMotionTranslation.x = model->GetNodes()[rootMotionHipNodeIndex].translate.x - cahcheRootMotionTranslation.x;
    //RootMotionTranslation.y = objectModel->GetNodes()[RootMotionNodeIndex].translate.y - CacheRootMotionTranslation.y;
    rootMotionTranslation.z = model->GetNodes()[rootMotionHipNodeIndex].translate.z - cahcheRootMotionTranslation.z;

    //����ɍ����ʂ����߂邽�߂ɍ���̈ړ��l���L���b�V������
    cahcheRootMotionTranslation = model->GetNodes()[rootMotionHipNodeIndex].translate;

    //�A�j���[�V�������ňړ����Ăق����Ȃ��̂Ń��[�g���[�V�����ړ��l�����Z�b�g
    model->GetNodes()[rootMotionHipNodeIndex].translate.x = 0.0f;
    model->GetNodes()[rootMotionHipNodeIndex].translate.z = 0.0f;

    //���[�g���[�V�����t���O���I�t�ɂ���
    rootMotionFlag = false;
}

//���[�g���[�V�����X�V
void AnimationCom::updateRootMotion(DirectX::XMFLOAT3& translation)
{
    Model* model = GetGameObject()->GetComponent<RendererCom>()->GetModel();

    if (rootMotionNodeIndex < 0)
    {
        return;
    }

    DirectX::XMMATRIX transform;

    DirectX::XMVECTOR tranlation = DirectX::XMLoadFloat3(&rootMotionTranslation);

    if (rootMotionNodeIndex == 0)
    {
        transform = DirectX::XMLoadFloat4x4(&model->GetNodes()[rootMotionNodeIndex].worldTransform);
    }
    else
    {
        transform = DirectX::XMLoadFloat4x4(&model->GetNodes()[rootMotionNodeIndex].worldTransform);

        DirectX::XMVECTOR position = DirectX::XMVector3TransformCoord(tranlation, transform);
        DirectX::XMStoreFloat3(&translation, position);

        GetGameObject()->transform_->SetWorldPosition(translation);

        rootMotionTranslation = { 0,0,0 };
    }
}