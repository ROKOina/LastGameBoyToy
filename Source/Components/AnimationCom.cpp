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

    switch (animaType)
    {
        //普通のアニメーション更新
    case AnimationType::NormalAnimation:
        //更新処理
        AnimationUpdata(elapsedTime);
        break;
        //上半身別アニメーション更新
    case AnimationType::UpperLowerAnimation:
        AnimationUpperUpdate(elapsedTime);
        AnimationLowerUpdate(elapsedTime);
        break;
        //上半身別上半身アニメーションブレンド更新
    case AnimationType::UpperBlendLowerAnimation:
        break;
    }

}

// GUI描画
bool isAnimLoop;
void AnimationCom::OnGUI()
{
    //if (!GetGameObject()->GetComponent<RendererCom>())return;

    const ModelResource* resource = GetGameObject()->GetComponent<RendererCom>()->GetModel()->GetResource();
    const std::vector<ModelResource::Animation>& animations = resource->GetAnimations();
    //モデルからリソースを取得
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

        // クリックすると選択
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



//Debug用選択アニメーションを取得
ModelResource::Animation* AnimationCom::GetSelectionAnimation()
{
    //モデルからリソースを取得
    Model* model = GetGameObject()->GetComponent<RendererCom>()->GetModel();

    const std::vector<ModelResource::Animation>& animations = model->GetResource()->GetAnimations();
    if (currentAnimation >= 0 && currentAnimation < static_cast<int>(animations.size()))
    {
        return const_cast<ModelResource::Animation*>(&animations.at(currentAnimation));
    }
    return nullptr;
}


//アニメーション更新
void AnimationCom::AnimationUpdata(float elapsedTime)
{
    //モデルからリソースを取得
    Model* model = GetGameObject()->GetComponent<RendererCom>()->GetModel();
    const ModelResource* resource = model->GetResource();


    //ブレンド率の計算
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
            //再生時間を巻き戻す
            currentSeconds -= animation.secondsLength;
            //ルートモーション用の計算
            if (rootMotionNodeIndex >= 0)
            {
                //零クリアするとHips分の値がずれるのでアニメーションの最初のフレームのHipsの値を入れて初期化
                cahcheRootMotionTranslation.x = animation.keyframes[0].nodeKeys[rootMotionHipNodeIndex].translate.x;
                cahcheRootMotionTranslation.z = animation.keyframes[0].nodeKeys[rootMotionHipNodeIndex].translate.z;

                //キャッシュルートモーションでクリア
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
        //ルートモーション計算
        ComputeRootMotion();
    }


}



//上半身別アニメーション更新
void AnimationCom::AnimationUpperUpdate(float elapsedTime)
{
    //モデルからリソースを取得
    Model* model = GetGameObject()->GetComponent<RendererCom>()->GetModel();

    //再生中できないなら処理しない
    if (!IsPlayUpperAnimation())return;

    //ブレンド率の計算
    float blendRate = 1.0f;
    if (upperAnimationChangeTime > 0)
    {
        upperAnimationChangeRate += upperAnimationChangeTime;
        blendRate = upperAnimationChangeRate;
        if (blendRate > 1.0f)blendRate = 1.0f;
    }

    //指定のアニメーションデータを取得
    const std::vector<ModelResource::Animation>& animations = model->GetResource()->GetAnimations();
    const ModelResource::Animation& animation = animations.at(currentUpperAnimation);

    //アニメーションデータからキーフレームデータリストを取得
    const std::vector<ModelResource::Keyframe>& Keyframes = animation.keyframes;
    int keyCount = static_cast<int>(Keyframes.size());
    for (int keyIndex = 0; keyIndex < keyCount - 1; ++keyIndex)
    {
        //現在の時間がどのキーフレームの間にいるか判定する
        const ModelResource::Keyframe& keyframe0 = Keyframes.at(keyIndex);
        const ModelResource::Keyframe& keyframe1 = Keyframes.at(keyIndex + 1);
        if (upperCurrentAnimationSeconds >= keyframe0.seconds && upperCurrentAnimationSeconds < keyframe1.seconds)
        {
            //再生時間とキーフレームの時間から補完率を算出する
            float rate = (upperCurrentAnimationSeconds - keyframe0.seconds) / (keyframe1.seconds - keyframe0.seconds);

            int nodeCount = static_cast<int>(model->GetNodes().size());
            for (int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
            {
                //2つのキーフレーム間の補完計算
                const ModelResource::NodeKeyData& key0 = keyframe0.nodeKeys.at(nodeIndex);
                const ModelResource::NodeKeyData& key1 = keyframe1.nodeKeys.at(nodeIndex);



                if (blendRate < 1.0f)
                {
                    if (model->GetNodes()[nodeIndex].layer == 2 && upperIsPlayAnimation)
                    {
                        //現在の姿勢と次のキーフレームとの姿勢の補完
                        ComputeSwitchAnimation(key1, blendRate, model->GetNodes()[nodeIndex]);
                    }
                }
                //通常の計算
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

    //時間経過
    upperCurrentAnimationSeconds+=elapsedTime* animation.animationspeed;

    //最終フレーム
    if (animationUpperEndFlag)
    {
        animationUpperEndFlag = false;
        currentUpperAnimation = -1;
        upperIsPlayAnimation = false;
        return;
    }

    //再生時間が終端時間を超えたら
    if (upperCurrentAnimationSeconds >= animation.secondsLength && animationUpperLoopFlag == false)
    {
        animationUpperEndFlag = true;
        upperComplementFlag = true;
    }
    
    //再生時間が終端時間を超えたら
    if (upperCurrentAnimationSeconds >= animation.secondsLength)
    {
        //再生時間を巻き戻す
        upperCurrentAnimationSeconds -= animation.secondsLength;
    }
}


//上半身別アニメーション更新
void AnimationCom::AnimationLowerUpdate(float elapsedTime)
{
    //モデルからリソースを取得
    Model* model = GetGameObject()->GetComponent<RendererCom>()->GetModel();

    //再生中できないなら処理しない
    if (!IsPlayLowerAnimation())return;

    //ブレンド率の計算
    float blendRate = 1.0f;
    if (lowerAnimationChangeTime > 0)
    {
        lowerAnimationChangeRate += lowerAnimationChangeTime;
        blendRate = lowerAnimationChangeRate;
        if (blendRate > 1.0f)blendRate = 1.0f;
    }

    //指定のアニメーションデータを取得
    const std::vector<ModelResource::Animation>& animations = model->GetResource()->GetAnimations();
    const ModelResource::Animation& animation = animations.at(currentLowerAnimation);

    //アニメーションデータからキーフレームデータリストを取得
    const std::vector<ModelResource::Keyframe>& Keyframes = animation.keyframes;
    int keyCount = static_cast<int>(Keyframes.size());
    for (int keyIndex = 0; keyIndex < keyCount - 1; ++keyIndex)
    {
        //現在の時間がどのキーフレームの間にいるか判定する
        const ModelResource::Keyframe& keyframe0 = Keyframes.at(keyIndex);
        const ModelResource::Keyframe& keyframe1 = Keyframes.at(keyIndex + 1);
        if (lowerCurrentAnimationSeconds >= keyframe0.seconds && lowerCurrentAnimationSeconds < keyframe1.seconds)
        {
            //再生時間とキーフレームの時間から補完率を算出する
            float rate = (lowerCurrentAnimationSeconds - keyframe0.seconds) / (keyframe1.seconds - keyframe0.seconds);

            int nodeCount = static_cast<int>(model->GetNodes().size());
            for (int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
            {
                //2つのキーフレーム間の補完計算
                const ModelResource::NodeKeyData& key0 = keyframe0.nodeKeys.at(nodeIndex);
                const ModelResource::NodeKeyData& key1 = keyframe1.nodeKeys.at(nodeIndex);



                if (blendRate < 1.0f)
                {
                    if (model->GetNodes()[nodeIndex].layer == 1 && lowerIsPlayAnimation)
                    {
                        //現在の姿勢と次のキーフレームとの姿勢の補完
                        ComputeSwitchAnimation(key1, blendRate, model->GetNodes()[nodeIndex]);
                    }
                }
                //通常の計算
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
   


    //時間経過
    lowerCurrentAnimationSeconds += elapsedTime * animation.animationspeed;

    //最終フレーム
    if (animationLowerEndFlag)
    {
        animationLowerEndFlag = false;
        currentLowerAnimation = -1;
        lowerIsPlayAnimation = false;
        return;
    }

    //再生時間が終端時間を超えたら
    if (lowerCurrentAnimationSeconds >= animation.secondsLength)
    {
        if (animationLowerLoopFlag)
        {
            //再生時間を巻き戻す
            lowerCurrentAnimationSeconds -= animation.secondsLength;
            //ルートモーション用の計算
            if (rootMotionNodeIndex >= 0)
            {
                //零クリアするとHips分の値がずれるのでアニメーションの最初のフレームのHipsの値を入れて初期化
                cahcheRootMotionTranslation.x = animation.keyframes[0].nodeKeys[rootMotionHipNodeIndex].translate.x;
                cahcheRootMotionTranslation.z = animation.keyframes[0].nodeKeys[rootMotionHipNodeIndex].translate.z;

                //キャッシュルートモーションでクリア
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
        //ルートモーション計算
        ComputeRootMotion();
    }
}


//上半身アニメーション再生中か？
bool AnimationCom::IsPlayUpperAnimation()
{
    //モデルからリソースを取得
    Model* model = GetGameObject()->GetComponent<RendererCom>()->GetModel();
    if (currentUpperAnimation < 0)return false;
    if (currentUpperAnimation >= model->GetResource()->GetAnimations().size())return false;
    return true;
}

//下半身アニメーション再生中か？
bool AnimationCom::IsPlayLowerAnimation()
{
    //モデルからリソースを取得
    Model* model = GetGameObject()->GetComponent<RendererCom>()->GetModel();
    if (currentLowerAnimation < 0)return false;
    if (currentLowerAnimation >= model->GetResource()->GetAnimations().size())return false;
    return true;
}

//普通のアニメーション再生関数
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

//上半身のみアニメーション再生関数
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

//下半身のみアニメーション再生関数
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

//アニメーションストップ
void AnimationCom::StopAnimation()
{
    currentAnimation = -1;
}

int AnimationCom::FindAnimation(const char* animeName)
{
    //モデルからリソースを取得
    Model* model = GetGameObject()->GetComponent<RendererCom>()->GetModel();
    const ModelResource* resource = model->GetResource();

    for (int i = 0; i < resource->GetAnimations().size(); i++) {
        if (animeName == resource->GetAnimations().at(i).name) {

            return i;
        }
    }

    assert(!"引数の名前のアニメーションは、「なにも無かった!!」");

    return -1;
}

//アニメーション更新切り替え
void AnimationCom::SetUpAnimationUpdate(int updateId)
{
    animaType = updateId;
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
        return;
    }
    if (rootMotionNodeIndex < 0)
    {
        return;
    }

    //前のフレームと今回のフレームの移動量データの差分量を求める
    rootMotionTranslation.x = model->GetNodes()[rootMotionHipNodeIndex].translate.x - cahcheRootMotionTranslation.x;
    //RootMotionTranslation.y = objectModel->GetNodes()[RootMotionNodeIndex].translate.y - CacheRootMotionTranslation.y;
    rootMotionTranslation.z = model->GetNodes()[rootMotionHipNodeIndex].translate.z - cahcheRootMotionTranslation.z;

    //次回に差分量を求めるために今回の移動値をキャッシュする
    cahcheRootMotionTranslation = model->GetNodes()[rootMotionHipNodeIndex].translate;


    //アニメーション内で移動してほしくないのでルートモーション移動値をリセット
    model->GetNodes()[rootMotionHipNodeIndex].translate.x = 0.0f;
    model->GetNodes()[rootMotionHipNodeIndex].translate.z = 0.0f;


    //ルートモーションフラグをオフにする
    rootMotionFlag = false;
}

//ルートモーション更新
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