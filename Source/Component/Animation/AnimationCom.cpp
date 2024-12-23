#include "AnimationCom.h"
#include "Component\Renderer\RendererCom.h"
#include "Component\System\TransformCom.h"
#include "Math\Mathf.h"
#include "Component\Character/CharacterCom.h"
#include "AimIKCom.h"
#include "Math/AnimationCalculation.h"
#include <imgui.h>
#include <cassert>

//シーケンサーエディタ
#include "SystemStruct/MySequence.h"
MySequence mySequence;
static int selectedEntry = -1;

// 開始処理
void AnimationCom::Start()
{
    //上半身と下半身仕分け
    SeparateNode();

    // sequence with default values
    mySequence.mFrameMin = 0;
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
    //ImGui::DragFloat("Speed",&animationSpeed,0.1f);
    //ImGui::SameLine();

    ImGui::Checkbox("isEventWindow", &isEventWindow);
    if (isEventWindow)AnimEventWindow();

    const ModelResource* resource = GetGameObject()->GetComponent<RendererCom>()->GetModel()->GetResource();
    const std::vector<ModelResource::Animation>& animations = resource->GetAnimations();
    //モデルからリソースを取得
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

        // クリックすると選択
        if (ImGui::IsItemClicked())
        {
            currentAnimation = animationIndex;
            selectionAnimation = GetSelectionAnimation();

            //アニメーションイベント設定
            selectedEntry = -1;
            //最大フレーム数を保存
            auto& anim = model->GetResource()->GetAnimations()[currentAnimation];
            int frameLength = static_cast<int>(anim.secondsLength * 60);
            mySequence.mFrameMax = frameLength;

            //アニメーションイベント登録
            mySequence.AllDeleteItem();
            int index = 0;
            for (auto& animEve : selectionAnimation->animationevents)
            {
                mySequence.AddTypeName(index, animEve.name);
                auto& event = mySequence.myItems.emplace_back();
                event.mFrameStart = animEve.startframe * 60;
                event.mFrameEnd = animEve.endframe * 60;
                event.mType = index;
                index++;
            }
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

void AnimationCom::AnimEventWindow()
{
    if (currentAnimation < 0)return;
    //モデルからリソースを取得
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

    //イベント追加,削除
    //アドラムダ
    auto& addIDRam = [](int iD)
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
    //デリートラムダ
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

    //再生情報
    ImGui::Checkbox("Loop", &isAnimLoop);
    ImGui::SameLine();

    if (ImGui::Button("Play"))
    {
        PlayAnimation(currentAnimation, isAnimLoop, false, 0.2f);
        //anim.animationspeed = 1;
    }
    ImGui::SameLine();

    ImGui::InputInt("Frame ", &animationCurrentFrame, 1, 100, ImGuiInputTextFlags_ReadOnly);
    ImGui::SameLine();

    ImGui::InputInt("Frame Max", &frameLength, 1, 100, ImGuiInputTextFlags_ReadOnly);

    //イベント詳細
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

        //名前
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
        SetAnimationSeconds(animationCurrentFrame / 60.0f);
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

    //セーブ
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
                    AnimationCalculation::Instance().ComputeSwitchAnimation(key1, blendRate, model->GetNodes()[nodeIndex]);
                }
                else
                {
                    //アニメーション計算
                    AnimationCalculation::Instance().ComputeAnimation(key0, key1, rate, model->GetNodes()[nodeIndex]);
                }

                //AimIKの更新
                if (GetGameObject()->GetComponent<AimIKCom>())
                {
                    GetGameObject()->GetComponent<AimIKCom>()->AimIK();
                }
            }

            break;
        }
    }

    // 最終フレーム処理
    if (endAnimation)
    {
        endAnimation = false;
        if (!isEventWindow)
            currentAnimation = -1;
        return;
    }

    // 時間経過
    if (!oneTimeStop)
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

            int upperNodeCount = static_cast<int>(upperNodes.size());
            for (int nodeIndex = 0, upperNodeIndex = 0; upperNodeIndex < upperNodeCount; ++nodeIndex)
            {
                if (upperNodes[upperNodeIndex]->nodeIndex != model->GetNodes()[nodeIndex].nodeIndex) {
                    continue;
                }
                //2つのキーフレーム間の補完計算
                const ModelResource::NodeKeyData& key0 = keyframe0.nodeKeys.at(nodeIndex);
                const ModelResource::NodeKeyData& key1 = keyframe1.nodeKeys.at(nodeIndex);

                if (blendRate < 1.0f)
                {
                    if (upperIsPlayAnimation)
                    {
                        //現在の姿勢と次のキーフレームとの姿勢の補完
                        AnimationCalculation::Instance().ComputeSwitchAnimation(key1, blendRate, *upperNodes[upperNodeIndex]);
                    }
                }
                //通常の計算
                else
                {
                    if (upperIsPlayAnimation)
                    {
                        AnimationCalculation::Instance().ComputeAnimation(key0, key1, rate, *upperNodes[upperNodeIndex]);
                    }
                }

                //AimIKの更新
                if (GetGameObject()->GetComponent<AimIKCom>())
                {
                    GetGameObject()->GetComponent<AimIKCom>()->AimIK();
                }

                upperNodeIndex++;
            }

            break;
        }
    }

    //時間経過
    upperCurrentAnimationSeconds += elapsedTime * animation.animationspeed;

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

//下半身別アニメーション更新
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
    const ModelResource::Animation& animationTwo = animations.at(lowerAnimationTwoIndex);
    const ModelResource::Animation& animationThree = animations.at(lowerAnimationThreeIndex);
    const ModelResource::Animation& animationFour = animations.at(lowerAnimationFourIndex);

    //アニメーションデータからキーフレームデータリストを取得
    const std::vector<ModelResource::Keyframe>& Keyframes = animation.keyframes;
    const std::vector<ModelResource::Keyframe>& TwoKeyframes = animationTwo.keyframes;
    const std::vector<ModelResource::Keyframe>& ThreeKeyframes = animationThree.keyframes;
    const std::vector<ModelResource::Keyframe>& FourKeyframes = animationFour.keyframes;
    int keyCount = static_cast<int>(Keyframes.size());
    for (int keyIndex = 0; keyIndex < keyCount - 1; ++keyIndex)
    {
        //現在の時間がどのキーフレームの間にいるか判定する
        const ModelResource::Keyframe& keyframe0 = Keyframes.at(keyIndex);
        const ModelResource::Keyframe& keyframe1 = Keyframes.at(keyIndex + 1);
        const ModelResource::Keyframe& walkFront = Keyframes.at(keyIndex + 1);
        const ModelResource::Keyframe& walkBack = TwoKeyframes.at(keyIndex + 1);
        const ModelResource::Keyframe& walkRight = ThreeKeyframes.at(keyIndex + 1);
        const ModelResource::Keyframe& walkLeft = FourKeyframes.at(keyIndex + 1);

        if (lowerCurrentAnimationSeconds >= keyframe0.seconds && lowerCurrentAnimationSeconds < keyframe1.seconds)
        {
            //再生時間とキーフレームの時間から補完率を算出する
            float rate = (lowerCurrentAnimationSeconds - keyframe0.seconds) / (keyframe1.seconds - keyframe0.seconds);

            int lowerNodeCount = static_cast<int>(lowerNodes.size());
            for (int nodeIndex = 0, lowerNodeIndex = 0; lowerNodeIndex < lowerNodeCount; ++nodeIndex)
            {
                if (lowerNodes[lowerNodeIndex]->nodeIndex != model->GetNodes()[nodeIndex].nodeIndex)
                {
                    continue;
                }

                //2つのキーフレーム間の補完計算
                if (lowerBlendType == 0)
                {
                    const ModelResource::NodeKeyData& key0 = keyframe0.nodeKeys.at(nodeIndex);
                    const ModelResource::NodeKeyData& key1 = keyframe1.nodeKeys.at(nodeIndex);

                    if (blendRate < 1.0f)
                    {
                        if (lowerIsPlayAnimation)
                        {
                            //現在の姿勢と次のキーフレームとの姿勢の補完
                            AnimationCalculation::Instance().ComputeSwitchAnimation(key1, blendRate, *lowerNodes[lowerNodeIndex]);
                        }
                    }
                    //通常の計算
                    else
                    {
                        if (lowerIsPlayAnimation)
                        {
                            AnimationCalculation::Instance().ComputeAnimation(key0, key1, rate, *lowerNodes[lowerNodeIndex]);
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
                            //現在の姿勢と次のキーフレームとの姿勢の補完
                            AnimationCalculation::Instance().ComputeSwitchAnimation(key1, blendRate, *lowerNodes[lowerNodeIndex]);
                        }
                    }
                    //通常の計算
                    else
                    {
                        if (lowerIsPlayAnimation)
                        {
                            AnimationCalculation::Instance().ComputeAnimation(key0, key1, lowerRate, *lowerNodes[lowerNodeIndex]);
                        }
                    }
                }
                //歩きモーション用
                else if (lowerBlendType == 2)
                {
                    float walkBlendRate = 0.0f;

                    float stickAngle = GetGameObject()->GetComponent<CharacterCom>()->GetStickAngle();

                    //右から上
                    if (stickAngle >= 0.0f && stickAngle < 90.0f)
                    {
                        walkBlendRate = stickAngle / 90.0f;

                        const ModelResource::NodeKeyData& key0 = walkRight.nodeKeys.at(nodeIndex);
                        const ModelResource::NodeKeyData& key1 = walkFront.nodeKeys.at(nodeIndex);

                        ComputeLowerAnimation(key0, key1, blendRate, walkBlendRate, lowerNodeIndex);
                    }
                    else if (stickAngle >= 90.0f && stickAngle < 180.0f)
                    {
                        const ModelResource::NodeKeyData& key0 = walkFront.nodeKeys.at(nodeIndex);
                        const ModelResource::NodeKeyData& key1 = walkLeft.nodeKeys.at(nodeIndex);

                        walkBlendRate = (stickAngle - 90.0f) / 90.0f;

                        ComputeLowerAnimation(key0, key1, blendRate, walkBlendRate, lowerNodeIndex);
                    }
                    else if (stickAngle >= 180.0f && stickAngle < 270.0f)
                    {
                        const ModelResource::NodeKeyData& key0 = walkLeft.nodeKeys.at(nodeIndex);
                        const ModelResource::NodeKeyData& key1 = walkBack.nodeKeys.at(nodeIndex);

                        walkBlendRate = (stickAngle - 180.0f) / 90.0f;

                        ComputeLowerAnimation(key0, key1, blendRate, walkBlendRate, lowerNodeIndex);
                    }
                    else if (stickAngle >= 270.0f && stickAngle < 360.0f)
                    {
                        const ModelResource::NodeKeyData& key0 = walkBack.nodeKeys.at(nodeIndex);
                        const ModelResource::NodeKeyData& key1 = walkRight.nodeKeys.at(nodeIndex);

                        walkBlendRate = (stickAngle - 270.0f) / 90.0f;

                        ComputeLowerAnimation(key0, key1, blendRate, walkBlendRate, lowerNodeIndex);
                    }
                }
                lowerNodeIndex++;
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
void AnimationCom::PlayLowerBodyOnlyAnimation(PlayLowBodyAnimParam param)
{
    currentLowerAnimation = param.lowerAnimaOneId; //前
    lowerAnimationTwoIndex = param.lowerAnimeTwoId;//後
    lowerAnimationThreeIndex = param.lowerAnimeThreeId;//右
    lowerAnimationFourIndex = param.lowerAnimeFourId;//左
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

bool AnimationCom::IsEventCalling(std::string eventName)
{
    if (currentAnimation < 0)return false;
    //モデルからリソースを取得
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
    //モデルからリソースを取得
    Model* model = GetGameObject()->GetComponent<RendererCom>()->GetModel();

    auto& anim = model->GetResource()->GetAnimations()[currentAnimation];

    for (auto& ev : anim.animationevents)
    {
        if (ev.name != eventName)continue;

        if (ev.startframe <= currentSeconds && ev.endframe >= currentSeconds)
        {
            //ノードのワールド位置を割り出す
            auto node = model->FindNode(nodeName.c_str());
            pos.x = node->worldTransform._41;
            pos.y = node->worldTransform._42;
            pos.z = node->worldTransform._43;

            return true;
        }
    }
    return false;
}

//下半身別アニメーション計算
void AnimationCom::ComputeLowerAnimation(const ModelResource::NodeKeyData& key0, const ModelResource::NodeKeyData& key1, float blendRate, float walkBlend, int index)
{
    if (blendRate < 1.0f)
    {
        if (lowerIsPlayAnimation)
        {
            //前回のアニメーションとのブレンド
            AnimationCalculation::Instance().ComputeWalkIdleAnimation(key0, key1, blendRate, walkBlend, *lowerNodes[index]);
        }
    }
    else
    {
        if (lowerIsPlayAnimation)
        {
            AnimationCalculation::Instance().ComputeAnimation(key0, key1, walkBlend, *lowerNodes[index]);
        }
    }
}

//子供をすべて保存
void RegisterChildLayer(std::vector<Model::Node*>& nodes, Model::Node& node)
{
    nodes.emplace_back(&node);
    for (auto& n : node.children)
    {
        RegisterChildLayer(nodes, *n);
        n->layer[0] = 0;
    }
}

//上半身と下半身のノードを分ける
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
            //上半身
            if (layer == 1) {
                RegisterChildLayer(upperNodes, node);
            }
            //下半身
            else if (layer == 2) {
                RegisterChildLayer(lowerNodes, node);
            }
        }
    }
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