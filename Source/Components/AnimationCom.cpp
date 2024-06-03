#include "AnimationCom.h"

#include "RendererCom.h"
#include "TransformCom.h"
#include <imgui.h>

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

    //const ModelResource* resource = GetGameObject()->GetComponent<RendererCom>()->GetModel()->GetResource();
    //const std::vector<ModelResource::Animation>& animations = resource->GetAnimations();

    //ImGui::Checkbox("animationLoop", &isAnimLoop);
    //ImGui::Separator();
    //int index = 0;
    //for (ModelResource::Animation anim : animations)
    //{
    //	index++;
    //}
}

//アニメーション更新
void AnimationCom::AnimationUpdata(float elapsedTime)
{
    //モデルからリソースを取得
    Model* model = GetGameObject()->GetComponent<RendererCom>()->GetModel();
    const ModelResource* resource = model->GetResource();

    model->UpdateAnimation(elapsedTime);
}