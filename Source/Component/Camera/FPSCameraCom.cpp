#include "FPSCameraCom.h"
#include "Graphics/Graphics.h"
#include "Component/System/TransformCom.h"
#include "Component/Character/CharacterCom.h"
#include "Component/MoveSystem/MovementCom.h"

//�R���X�g���N�^
FPSCameraCom::FPSCameraCom() : CameraCom::CameraCom(80, Graphics::Instance().GetScreenWidth() / Graphics::Instance().GetScreenHeight(), 0.1f, 1000.0f)
{
    focuslapelate = 0.93f;
    eyelaperate = 0.01f;
}

// �J�n����
void FPSCameraCom::Start()
{
}

//�X�V����
void FPSCameraCom::Update(float elapsedTime)
{
    //���N���X�̃J�������Ă�
    CameraCom::Update(elapsedTime);
}

//imgui
void FPSCameraCom::OnGUI()
{
    //���N���X�̃J�������Ă�
    CameraCom::OnGUI();
}