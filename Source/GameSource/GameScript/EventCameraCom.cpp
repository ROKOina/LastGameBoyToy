#include "EventCameraCom.h"
#include "Graphics/Graphics.h"
#include "Components/TransformCom.h"
#include "Components/Character/CharacterCom.h"
#include "Components/MovementCom.h"

//�R���X�g���N�^
EventCameraCom::EventCameraCom() : CameraCom::CameraCom(45, Graphics::Instance().GetScreenWidth() / Graphics::Instance().GetScreenHeight(), 0.1f, 1000.0f)
{
    //focuslapelate = 0.93f;
    //eyelaperate = 0.01f;
}

// �J�n����
void EventCameraCom::Start()
{
}

//�X�V����
void EventCameraCom::Update(float elapsedTime)
{
    //���N���X�̃J�������Ă�
    CameraCom::Update(elapsedTime);
}

//imgui
void EventCameraCom::OnGUI()
{
    //���N���X�̃J�������Ă�
    CameraCom::OnGUI();
}