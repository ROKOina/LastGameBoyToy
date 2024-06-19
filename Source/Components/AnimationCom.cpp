#include "AnimationCom.h"

#include "RendererCom.h"
#include "TransformCom.h"
#include <imgui.h>
#include <cassert>

// �J�n����
void AnimationCom::Start()
{
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
            PlayAnimation(currentAnimation, isAnimLoop, false,0.2f);
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
                    if (model->GetNodes()[nodeIndex].layer == 2 && upperIsPlayAnimation)
                    {
                        //���݂̎p���Ǝ��̃L�[�t���[���Ƃ̎p���̕⊮
                        ComputeSwitchAnimation(key1, blendRate, model->GetNodes()[nodeIndex]);
                    }
                }
                //�ʏ�̌v�Z
                else
                {
                    if (model->GetNodes()[nodeIndex].layer == 2 && upperIsPlayAnimation)
                    {
                        ComputeAnimation(key0, key1, rate, model->GetNodes()[nodeIndex]);
                    }
                }

            }
            break;
        }
    }

    //���Ԍo��
    upperCurrentAnimationSeconds+=elapsedTime* animation.animationspeed;

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


//�㔼�g�ʃA�j���[�V�����X�V
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

    //�A�j���[�V�����f�[�^����L�[�t���[���f�[�^���X�g���擾
    const std::vector<ModelResource::Keyframe>& Keyframes = animation.keyframes;
    int keyCount = static_cast<int>(Keyframes.size());
    for (int keyIndex = 0; keyIndex < keyCount - 1; ++keyIndex)
    {
        //���݂̎��Ԃ��ǂ̃L�[�t���[���̊Ԃɂ��邩���肷��
        const ModelResource::Keyframe& keyframe0 = Keyframes.at(keyIndex);
        const ModelResource::Keyframe& keyframe1 = Keyframes.at(keyIndex + 1);
        if (lowerCurrentAnimationSeconds >= keyframe0.seconds && lowerCurrentAnimationSeconds < keyframe1.seconds)
        {
            //�Đ����ԂƃL�[�t���[���̎��Ԃ���⊮�����Z�o����
            float rate = (lowerCurrentAnimationSeconds - keyframe0.seconds) / (keyframe1.seconds - keyframe0.seconds);

            int nodeCount = static_cast<int>(model->GetNodes().size());
            for (int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
            {
                //2�̃L�[�t���[���Ԃ̕⊮�v�Z
                const ModelResource::NodeKeyData& key0 = keyframe0.nodeKeys.at(nodeIndex);
                const ModelResource::NodeKeyData& key1 = keyframe1.nodeKeys.at(nodeIndex);



                if (blendRate < 1.0f)
                {
                    if (model->GetNodes()[nodeIndex].layer == 1 && lowerIsPlayAnimation)
                    {
                        //���݂̎p���Ǝ��̃L�[�t���[���Ƃ̎p���̕⊮
                        ComputeSwitchAnimation(key1, blendRate, model->GetNodes()[nodeIndex]);
                    }
                }
                //�ʏ�̌v�Z
                else
                {
                    if (model->GetNodes()[nodeIndex].layer == 1 && lowerIsPlayAnimation)
                    {
                        ComputeAnimation(key0, key1, rate, model->GetNodes()[nodeIndex]);
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
void AnimationCom::PlayAnimation(int animeID, bool loop,bool rootFlag, float blendSeconds)
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
void AnimationCom::PlayLowerBodyOnlyAnimation(int lowerAnimaId, bool loop, bool rootFlga, float blendSeconds)
{
    currentLowerAnimation = lowerAnimaId;
    lowerCurrentAnimationSeconds = 0.0f;
    animationLowerLoopFlag = loop;
    animationLowerEndFlag = false;
    lowerAnimationChangeTime = blendSeconds;
    lowerIsPlayAnimation = true;
    beforeOneFream = false;
    lowerAnimationChangeRate = 0.0f;
    this->rootFlag = rootFlga;
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


    DirectX::XMVECTOR S = DirectX::XMVectorLerp(MS1,S1, blendRate);
    DirectX::XMVECTOR R = DirectX::XMQuaternionSlerp(MR1,R1, blendRate);
    DirectX::XMVECTOR T = DirectX::XMVectorLerp(MT1,T1, blendRate);

    DirectX::XMStoreFloat3(&node.scale, S);
    DirectX::XMStoreFloat4(&node.rotate, R);
    DirectX::XMStoreFloat3(&node.translate, T);
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

    if (rootMotionNodeIndex <0)
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