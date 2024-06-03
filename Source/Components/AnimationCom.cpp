#include "AnimationCom.h"

#include "RendererCom.h"
#include "TransformCom.h"
#include <imgui.h>

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

//�A�j���[�V�����X�V
void AnimationCom::AnimationUpdata(float elapsedTime)
{
    //���f�����烊�\�[�X���擾
    Model* model = GetGameObject()->GetComponent<RendererCom>()->GetModel();
    const ModelResource* resource = model->GetResource();

    model->UpdateAnimation(elapsedTime);
}