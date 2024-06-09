#include "AnimationCom.h"

#include "RendererCom.h"
#include "TransformCom.h"
#include <imgui.h>
#include <cassert>

// 開始処理
void AnimationCom::Start()
{
}

// 更新処理
void AnimationCom::Update(float elapsedTime)
{
    AnimationUpdata(elapsedTime);
}

// GUI描画
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

//アニメーション更新
void AnimationCom::AnimationUpdata(float elapsedTime)
{
    //モデルからリソースを取得
    Model* model = GetGameObject()->GetComponent<RendererCom>()->GetModel();
    const ModelResource* resource = model->GetResource();


    //ブレンド率の計算
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
        // 現在の時間がどのキーフレームの間にいるか判定する
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
                // ２つのキーフレーム間の補完計算
                const ModelResource::NodeKeyData& key0 = keyframe0.nodeKeys.at(nodeIndex);
                const ModelResource::NodeKeyData& key1 = keyframe1.nodeKeys.at(nodeIndex);

                 
                if (blendRate < 1.0f)
                {
                    //アニメーション切り替え時の計算
                    ComputeSwitchAnimation(key1, blendRate, model->GetNodes()[nodeIndex]);
                }
                else
                {
                    //アニメーション計算
                    ComputeAnimation(key0, key1, rate, model->GetNodes()[nodeIndex]);
                }
            }
            break;
        }
    }

    // 最終フレーム処理
    if (endAnimation)
    {
        endAnimation = false;
        currentAnimation = -1;
        return;
    }


    // 時間経過
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



//普通のアニメーション再生関数
void AnimationCom::PlayAnimation(int animeID, bool loop, float blendSeconds)
{
    currentAnimation = animeID;
    loopAnimation = loop;
    endAnimation = false;
    currentSeconds = 0.0f;
    animationChangeTime = blendSeconds;
    animationChangeRate = 0.0f;
}

//アニメーションストップ
void AnimationCom::StopAnimation()
{
    currentAnimation = -1;
}

//アニメーション計算
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

//アニメーション切り替え時の計算
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




//ルートモーションの値を取るノードを検索
void AnimationCom::SetupRootMotion(const char* rootMotionNodeIndex)
{
    Model* model = GetGameObject()->GetComponent<RendererCom>()->GetModel();
    this->rootMotionNodeIndex = model->FindNodeIndex(rootMotionNodeIndex);
}

//ルートモーションの腰を取るノードを検索
void AnimationCom::SetupRootMotionHip(const char* rootMotionNodeName)
{
    Model* model = GetGameObject()->GetComponent<RendererCom>()->GetModel();
    this->rootMotionHipNodeIndex = model->FindNodeIndex(rootMotionNodeName);
}

//ルートモーションの移動値を計算
void AnimationCom::ComputeRootMotion()
{
    Model* model = GetGameObject()->GetComponent<RendererCom>()->GetModel();



    if (!rootMotionFlag)
    {
        //ルートフラグが無かったらしない
        return;
    }
    if (rootMotionNodeIndex < 0)
    {
        //ルートインデックスが無かったらしない
        return;
    }

    //前のフレームと今回のフレームの移動量データの差分量を求める
    rootMotionTranslation.x = model->GetNodes()[rootMotionHipNodeIndex].translate.x - cahcheRootMotionTranslation.x;
    rootMotionTranslation.z = model->GetNodes()[rootMotionHipNodeIndex].translate.z - cahcheRootMotionTranslation.z;


    //次回に差分量を求めるために今回の移動値をキャッシュする
    cahcheRootMotionTranslation = model->GetNodes()[rootMotionHipNodeIndex].translate;

    //アニメーション内で移動してほしくないのでルートモーション移動値をリセット
    model->GetNodes()[rootMotionHipNodeIndex].translate.x = 0;
    model->GetNodes()[rootMotionHipNodeIndex].translate.z = 0;

    //ルートモーションフラグをオフにする
    rootMotionFlag = false;
}

//ルートモーション更新
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