#include "AnimationCom.h"
#include "RendererCom.h"
#include "TransformCom.h"
#include "../GameSource/Math/Mathf.h"
#include "Character/CharacterCom.h"
#include "Components/AimIKCom.h"
#include <imgui.h>
#include <cassert>

//�V�[�P���T�[�G�f�B�^
#include "SystemStruct/MySequence.h"
MySequence mySequence;
static int selectedEntry = -1;

// �J�n����
void AnimationCom::Start()
{
    //�㔼�g�Ɖ����g�d����
    SeparateNode();

    // sequence with default values
    mySequence.mFrameMin = 0;
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
    ImGui::SameLine();

    ImGui::Checkbox("isEventWindow", &isEventWindow);
    if (isEventWindow)AnimEventWindow();

    const ModelResource* resource = GetGameObject()->GetComponent<RendererCom>()->GetModel()->GetResource();
    const std::vector<ModelResource::Animation>& animations = resource->GetAnimations();
    //���f�����烊�\�[�X���擾
    Model* model = GetGameObject()->GetComponent<RendererCom>()->GetModel();

    //ImGui::Checkbox("animationLoop", &isAnimLoop);
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
        //ImGui::DragFloat(animation.name.c_str(), &animation.animationspeed);

        ImGui::TreeNodeEx(&animation, nodeFlags, animation.name.c_str());

        // �N���b�N����ƑI��
        if (ImGui::IsItemClicked())
        {
            currentAnimation = animationIndex;
            selectionAnimation = GetSelectionAnimation();

            //�A�j���[�V�����C�x���g�ݒ�
            selectedEntry = -1;
            //�ő�t���[������ۑ�
            auto& anim = model->GetResource()->GetAnimations()[currentAnimation];
            int frameLength = static_cast<int>(anim.secondsLength * 60);
            mySequence.mFrameMax = frameLength;

            //�A�j���[�V�����C�x���g�o�^
            mySequence.AllDeleteItem();
            int index = 0;
            for (auto& animEve : selectionAnimation->animationevents)
            {
                mySequence.AddTypeName(index, animEve.name);
                auto& event = mySequence.myItems.emplace_back();
                event.mFrameStart = animEve.startframe;
                event.mFrameEnd = animEve.endframe;
                event.mType = index;
                index++;
            }
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

void AnimationCom::AnimEventWindow()
{
    if (currentAnimation < 0)return;
    //���f�����烊�\�[�X���擾
    Model* model = GetGameObject()->GetComponent<RendererCom>()->GetModel();
    auto& anim = model->GetResource()->GetAnimations()[currentAnimation];


    ImGui::SetNextWindowPos(ImVec2(320, 600), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(630, 100), ImGuiCond_FirstUseEver);

    ImGui::Begin("AnimEvent", nullptr, ImGuiWindowFlags_None);



    float animationCurrentSeconds = GetAnimationSeconds();
    int animationCurrentFrame = static_cast<int>(animationCurrentSeconds * 60.0f);
    int frameLength = static_cast<int>(anim.secondsLength * 60);

    // let's create the sequencer
    static int firstFrame = 0;
    static bool expanded = true;

    ImGui::PushItemWidth(130);

    //�C�x���g�ǉ�,�폜
    //�A�h�����_
    auto& addIDRam= [](int iD)
        {
            int index = 0;
            while (1)
            {
                auto& it = mySequence.SequencerItemTypeNames.find(index);
                if (it == mySequence.SequencerItemTypeNames.end())break;
                index++;
            }
            mySequence.AddTypeName(index, std::string("NEW") + std::to_string(index));
            int s = 0;
            int e = 10;
            if (iD >= 0)
            {
                s = mySequence.myItems[iD].mFrameStart;
                e = mySequence.myItems[iD].mFrameEnd;
            }
            mySequence.myItems.push_back(MySequence::MySequenceItem{ index, s, e, false });
        };
    if (ImGui::Button("Add"))
    {
        addIDRam(-1);
    }
    ImGui::SameLine();
    //�f���[�g�����_
    auto& deleteIDRam = [](int iD)
        {
            if (iD != -1)
            {
                int typeID = mySequence.Delete(iD);
                mySequence.DeleteItem(typeID);
                if (mySequence.GetItemCount() == selectedEntry)selectedEntry -= 1;
                if (mySequence.GetItemCount() == 0)selectedEntry = -1;
            }
        };
    if (ImGui::Button("Delete"))
    {
        deleteIDRam(selectedEntry);
    }

    //�Đ����
    ImGui::Checkbox("Loop", &isAnimLoop);
    ImGui::SameLine();

    if (ImGui::Button("Play"))
    {
        PlayAnimation(currentAnimation, isAnimLoop, false, 0.2f);
        animationSpeed = 1;
    }
    ImGui::SameLine();

    ImGui::InputInt("Frame ", &animationCurrentFrame, 1, 100, ImGuiInputTextFlags_ReadOnly);
    ImGui::SameLine();

    ImGui::InputInt("Frame Max", &frameLength, 1, 100, ImGuiInputTextFlags_ReadOnly);

    //�C�x���g�ڍ�
    if (selectedEntry >= 0)
    {
        MySequence::MySequenceItem& nowItem = mySequence.myItems[selectedEntry];
        if (ImGui::Button("Start"))
        {
            nowItem.mFrameStart = animationCurrentFrame;
        }
        ImGui::SameLine();
        ImGui::DragInt("StartFrame", &nowItem.mFrameStart);
        ImGui::SameLine();
        if (ImGui::Button("End"))
        {
            nowItem.mFrameEnd = animationCurrentFrame;
        }
        ImGui::SameLine();
        ImGui::DragInt("EndFrame", &nowItem.mFrameEnd);
        ImGui::SameLine();


        //���O
        char name[256];
        ::strcpy_s(name, sizeof(name), mySequence.GetItemLabel(selectedEntry));
        if (ImGui::InputText("Name", name, ImGuiInputTextFlags_EnterReturnsTrue))
        {
            mySequence.SequencerItemTypeNames[selectedEntry] = name;
        }
    }

    ImGui::PopItemWidth();

    static bool moveFrame = false;
    int delID = -1;
    int addID = -1;
    Sequencer(&mySequence, &animationCurrentFrame, &expanded, &selectedEntry, &firstFrame, ImSequencer::SEQUENCER_EDIT_STARTEND | ImSequencer::SEQUENCER_DEL | ImSequencer::SEQUENCER_CHANGE_FRAME
        , moveFrame, delID, addID);
    if (moveFrame)
    {
        PlayAnimation(currentAnimation, isAnimLoop, false, 0.2f);
        SetAnimationSeconds(animationCurrentFrame/60.0f);
        StopOneTimeAnimation();

    }
    if (delID >= 0)
    {
        deleteIDRam(delID);
    }
    if (addID >= 0)
    {
        addIDRam(addID);
    }
    
    if (selectedEntry != -1)
    {
        const MySequence::MySequenceItem& item = mySequence.myItems[selectedEntry];

    }

    //�Z�[�u
    if (ImGui::Button("Save"))
    {
        ModelResource::Animation* selectionAnimation = GetSelectionAnimation();
        selectionAnimation->animationevents.clear();
        for (auto& item : mySequence.myItems)
        {
            auto& event = selectionAnimation->animationevents.emplace_back();
            event.name = mySequence.GetItemLabel(item.mType);
            event.endframe = item.mFrameEnd / 60.0f;
            event.startframe = item.mFrameStart / 60.0f;
        }
        model->GetResource()->AnimSerialize();
    }

    ImGui::End();
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

                //AimIK�̍X�V
                if (GetGameObject()->GetComponent<AimIKCom>())
                {
                    GetGameObject()->GetComponent<AimIKCom>()->AimIK();
                }
            }

            break;
        }
    }

    // �ŏI�t���[������
    if (endAnimation)
    {
        endAnimation = false;
        if (!isEventWindow)
            currentAnimation = -1;
        return;
    }

    // ���Ԍo��
    if (!oneTimeStop)
        currentSeconds += elapsedTime * animationSpeed;
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

            int upperNodeCount = static_cast<int>(upperNodes.size());
            for (int nodeIndex = 0, upperNodeIndex = 0; upperNodeIndex < upperNodeCount; ++nodeIndex)
            {
                if (upperNodes[upperNodeIndex]->nodeIndex != model->GetNodes()[nodeIndex].nodeIndex) {
                    continue;
                }
                //2�̃L�[�t���[���Ԃ̕⊮�v�Z
                const ModelResource::NodeKeyData& key0 = keyframe0.nodeKeys.at(nodeIndex);
                const ModelResource::NodeKeyData& key1 = keyframe1.nodeKeys.at(nodeIndex);

                if (blendRate < 1.0f)
                {
                    if (upperIsPlayAnimation)
                    {
                        //���݂̎p���Ǝ��̃L�[�t���[���Ƃ̎p���̕⊮
                        ComputeSwitchAnimation(key1, blendRate, *upperNodes[upperNodeIndex]);
                    }
                }
                //�ʏ�̌v�Z
                else
                {
                    if (upperIsPlayAnimation)
                    {
                        ComputeAnimation(key0, key1, rate, *upperNodes[upperNodeIndex]);
                    }
                }

                //AimIK�̍X�V
                if (GetGameObject()->GetComponent<AimIKCom>())
                {
                    GetGameObject()->GetComponent<AimIKCom>()->AimIK();
                }

                upperNodeIndex++;
            }

            break;
        }
    }

    //���Ԍo��
    upperCurrentAnimationSeconds += elapsedTime * animationSpeed;

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
        const ModelResource::Keyframe& walkBack = TwoKeyframes.at(keyIndex + 1);
        const ModelResource::Keyframe& walkRight = ThreeKeyframes.at(keyIndex + 1);
        const ModelResource::Keyframe& walkLeft = FourKeyframes.at(keyIndex + 1);

        if (lowerCurrentAnimationSeconds >= keyframe0.seconds && lowerCurrentAnimationSeconds < keyframe1.seconds)
        {
            //�Đ����ԂƃL�[�t���[���̎��Ԃ���⊮�����Z�o����
            float rate = (lowerCurrentAnimationSeconds - keyframe0.seconds) / (keyframe1.seconds - keyframe0.seconds);

            int lowerNodeCount = static_cast<int>(lowerNodes.size());
            for (int nodeIndex = 0, lowerNodeIndex = 0; lowerNodeIndex < lowerNodeCount; ++nodeIndex)
            {
                if (lowerNodes[lowerNodeIndex]->nodeIndex != model->GetNodes()[nodeIndex].nodeIndex)
                {
                    continue;
                }

                //2�̃L�[�t���[���Ԃ̕⊮�v�Z
                if (lowerBlendType == 0)
                {
                    const ModelResource::NodeKeyData& key0 = keyframe0.nodeKeys.at(nodeIndex);
                    const ModelResource::NodeKeyData& key1 = keyframe1.nodeKeys.at(nodeIndex);

                    if (blendRate < 1.0f)
                    {
                        if (lowerIsPlayAnimation)
                        {
                            //���݂̎p���Ǝ��̃L�[�t���[���Ƃ̎p���̕⊮
                            ComputeSwitchAnimation(key1, blendRate, *lowerNodes[lowerNodeIndex]);
                        }
                    }
                    //�ʏ�̌v�Z
                    else
                    {
                        if (lowerIsPlayAnimation)
                        {
                            ComputeAnimation(key0, key1, rate, *lowerNodes[lowerNodeIndex]);
                        }
                    }
                }
                else if (lowerBlendType == 1)
                {
                    const ModelResource::NodeKeyData& key0 = keyframe0.nodeKeys.at(nodeIndex);
                    const ModelResource::NodeKeyData& key1 = walkFront.nodeKeys.at(nodeIndex);

                    if (blendRate < 1.0f)
                    {
                        if (lowerIsPlayAnimation)
                        {
                            //���݂̎p���Ǝ��̃L�[�t���[���Ƃ̎p���̕⊮
                            ComputeSwitchAnimation(key1, blendRate, *lowerNodes[lowerNodeIndex]);
                        }
                    }
                    //�ʏ�̌v�Z
                    else
                    {
                        if (lowerIsPlayAnimation)
                        {
                            ComputeAnimation(key0, key1, lowerRate, *lowerNodes[lowerNodeIndex]);
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
                            if (lowerIsPlayAnimation)
                            {
                                //�O��̃A�j���[�V�����Ƃ̃u�����h
                                ComputeWalkIdleAnimation(key0, key1, blendRate, walkBlendRate, *lowerNodes[lowerNodeIndex]);
                            }
                        }
                        else
                        {
                            if (lowerIsPlayAnimation)
                            {
                                ComputeAnimation(key0, key1, walkBlendRate, *lowerNodes[lowerNodeIndex]);
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
                            if (lowerIsPlayAnimation)
                            {
                                //�O��̃A�j���[�V�����Ƃ̃u�����h
                                ComputeWalkIdleAnimation(key0, key1, blendRate, walkBlendRate, *lowerNodes[lowerNodeIndex]);
                            }
                        }
                        else
                        {
                            if (lowerIsPlayAnimation)
                            {
                                ComputeAnimation(key0, key1, walkBlendRate, *lowerNodes[lowerNodeIndex]);
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
                            if (lowerIsPlayAnimation)
                            {
                                //�O��̃A�j���[�V�����Ƃ̃u�����h
                                ComputeWalkIdleAnimation(key0, key1, blendRate, walkBlendRate, *lowerNodes[lowerNodeIndex]);
                            }
                        }
                        else
                        {
                            if (lowerIsPlayAnimation)
                            {
                                ComputeAnimation(key0, key1, walkBlendRate, *lowerNodes[lowerNodeIndex]);
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
                            if (lowerIsPlayAnimation)
                            {
                                //�O��̃A�j���[�V�����Ƃ̃u�����h
                                ComputeWalkIdleAnimation(key0, key1, blendRate, walkBlendRate, *lowerNodes[lowerNodeIndex]);
                            }
                        }
                        else
                        {
                            if (lowerIsPlayAnimation)
                            {
                                ComputeAnimation(key0, key1, walkBlendRate, *lowerNodes[lowerNodeIndex]);
                            }
                        }
                    }
                }
                lowerNodeIndex++;
            }

            break;
        }
    }

    //���Ԍo��
    lowerCurrentAnimationSeconds += elapsedTime * animationSpeed;

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
    oneTimeStop = false;
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
void AnimationCom::PlayLowerBodyOnlyAnimation(PlayLowBodyAnimParam param)
{
    currentLowerAnimation = param.lowerAnimaOneId; //�O
    lowerAnimationTwoIndex = param.lowerAnimeTwoId;//��
    lowerAnimationThreeIndex = param.lowerAnimeThreeId;//�E
    lowerAnimationFourIndex = param.lowerAnimeFourId;//��
    lowerCurrentAnimationSeconds = 0.0f;
    animationLowerLoopFlag = param.loop;
    animationLowerEndFlag = false;
    lowerAnimationChangeTime = param.animeChangeRate;
    lowerIsPlayAnimation = true;
    beforeOneFream = false;
    lowerAnimationChangeRate = 0.0f;
    this->rootFlag = param.rootFlag;
    lowerBlendType = param.blendType;
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

bool AnimationCom::IsEventCalling(std::string eventName)
{
    if (currentAnimation < 0)return false;
    //���f�����烊�\�[�X���擾
    Model* model = GetGameObject()->GetComponent<RendererCom>()->GetModel();

    auto& anim = model->GetResource()->GetAnimations()[currentAnimation];

    for (auto& ev : anim.animationevents)
    {
        if (ev.name != eventName)continue;

        if (ev.startframe <= currentSeconds && ev.endframe >= currentSeconds)
            return true;
    }
    return false;
}

bool AnimationCom::IsEventCallingNodePos(std::string eventName, std::string nodeName, DirectX::XMFLOAT3& pos)
{
    if (currentAnimation < 0)return false;
    //���f�����烊�\�[�X���擾
    Model* model = GetGameObject()->GetComponent<RendererCom>()->GetModel();

    auto& anim = model->GetResource()->GetAnimations()[currentAnimation];

    for (auto& ev : anim.animationevents)
    {
        if (ev.name != eventName)continue;

        if (ev.startframe <= currentSeconds && ev.endframe >= currentSeconds)
        {
            //�m�[�h�̃��[���h�ʒu������o��
            auto node = model->FindNode(nodeName.c_str());
            pos.x = node->worldTransform._41;
            pos.y = node->worldTransform._42;
            pos.z = node->worldTransform._43;

            return true;
        }
    }
    return false;
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

//�q�������ׂĕۑ�
void RegisterChildLayer(std::vector<Model::Node*>& nodes, Model::Node& node)
{
    nodes.emplace_back(&node);
    for (auto& n : node.children)
    {
        RegisterChildLayer(nodes, *n);
        n->layer[0] = 0;
    }
}

//�㔼�g�Ɖ����g�̃m�[�h�𕪂���
void AnimationCom::SeparateNode()
{
    Model* model = GetGameObject()->GetComponent<RendererCom>()->GetModel();
    for (auto& node : model->GetNodes())
    {
        if (std::string(node.name).find("Hip") != std::string::npos)
        //if (std::strcmp(node.name, "Hip") == 0)
        {
            lowerNodes.emplace_back(&node);
            continue;
        }

        for (auto& layer : node.layer)
        {
            //�㔼�g
            if (layer == 1) {
                RegisterChildLayer(upperNodes, node);
            }
            //�����g
            else if (layer == 2) {
                RegisterChildLayer(lowerNodes, node);
            }
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