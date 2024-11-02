#include "EventCameraCom.h"
#include "Graphics/Graphics.h"
#include "Component/System/TransformCom.h"
#include "Component/Character/CharacterCom.h"
#include "Component/MoveSystem/MovementCom.h"
#include <Scene\SceneManager.h>

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

    static bool isEve = false;
    if (GetIsPlayEvent())
    {
        if (!isEve)
        {
            cameraCopy = SceneManager::Instance().GetActiveCamera();
            ActiveCameraChange();
            isEve = true;
        }
    }
    else
    {
        if (isEve)
        {
            cameraCopy.lock()->GetComponent<CameraCom>()->ActiveCameraChange();
            isEve = false;
        }
    }
}

//imgui
void EventCameraCom::OnGUI()
{
    //���N���X�̃J�������Ă�
    CameraCom::OnGUI();
}