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
    AnimationUpdata(elapsedTime);
}

// GUI�`��
bool isAnimLoop;
void AnimationCom::OnGUI()
{
    //if (!GetGameObject()->GetComponent<RendererCom>())return;

    const ModelResource* resource = GetGameObject()->GetComponent<RendererCom>()->GetModel()->GetResource();
    const std::vector<ModelResource::Animation>& animations = resource->GetAnimations();

    ImGui::Checkbox("animationLoop", &isAnimLoop);
    ImGui::Separator();
    int index = 0;
    for (ModelResource::Animation anim : animations)
    {
    	index++;
    }
}

//�A�j���[�V�����X�V
void AnimationCom::AnimationUpdata(float elapsedTime)
{
    //���f�����烊�\�[�X���擾
    Model* model = GetGameObject()->GetComponent<RendererCom>()->GetModel();
    const ModelResource* resource = model->GetResource();


    //�u�����h���̌v�Z
    float blendRate = 1.0f;
    if (animationChangeTime>0)
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
            currentSeconds -= animation.secondsLength;
        }
        else
        {
            currentSeconds = animation.secondsLength;
            endAnimation = true;
        }
    }

    
}



//���ʂ̃A�j���[�V�����Đ��֐�
void AnimationCom::PlayAnimation(int animeID, bool loop, float blendSeconds)
{
    currentAnimation = animeID;
    loopAnimation = loop;
    endAnimation = false;
    currentSeconds = 0.0f;
    animationChangeTime = blendSeconds;
    animationChangeRate = 0.0f;
}

//�A�j���[�V�����X�g�b�v
void AnimationCom::StopAnimation()
{
    currentAnimation = -1;
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
        //���[�g�t���O�����������炵�Ȃ�
        return;
    }
    if (rootMotionNodeIndex < 0)
    {
        //���[�g�C���f�b�N�X�����������炵�Ȃ�
        return;
    }

    //�O�̃t���[���ƍ���̃t���[���̈ړ��ʃf�[�^�̍����ʂ����߂�
    rootMotionTranslation.x = model->GetNodes()[rootMotionHipNodeIndex].translate.x - cahcheRootMotionTranslation.x;
    rootMotionTranslation.z = model->GetNodes()[rootMotionHipNodeIndex].translate.z - cahcheRootMotionTranslation.z;


    //����ɍ����ʂ����߂邽�߂ɍ���̈ړ��l���L���b�V������
    cahcheRootMotionTranslation = model->GetNodes()[rootMotionHipNodeIndex].translate;

    //�A�j���[�V�������ňړ����Ăق����Ȃ��̂Ń��[�g���[�V�����ړ��l�����Z�b�g
    model->GetNodes()[rootMotionHipNodeIndex].translate.x = 0;
    model->GetNodes()[rootMotionHipNodeIndex].translate.z = 0;

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
        transform = DirectX::XMLoadFloat4x4(&model->GetNodes()[rootMotionNodeIndex].parent->worldTransform);

        DirectX::XMVECTOR position = DirectX::XMVector3TransformCoord(tranlation, transform);
        DirectX::XMStoreFloat3(&translation, position);
        //GetGameObject().lock()->SetPosition(translation);
        GetGameObject()->transform_->SetWorldPosition(translation);

        rootMotionTranslation = { 0,0,0 };
    }
}