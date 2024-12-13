#include "BaseCharacterState.h"
#include "Input\Input.h"
#include "Component/Collsion/ColliderCom.h"
#include "Component/Renderer/RendererCom.h"
#include "Component\Bullet\BulletCom.h"
#include "Component\Particle\CPUParticle.h"
#include "Component\Particle\GPUParticle.h"
#include "Component\Character\RemoveTimerCom.h"
#include "Component\Camera\CameraCom.h"
#include <Component\Camera\EventCameraManager.h>
#include "Scene/SceneTitle/SceneTitle.h"
#include "Component\Audio\AudioCom.h"
#include "Component\GameSystem\RespawnCom.h"

//���N
BaseCharacter_BaseState::BaseCharacter_BaseState(CharacterCom* owner) : State(owner)
{
    //�����ݒ�
    charaCom = GetComp(CharacterCom);
    moveCom = GetComp(MovementCom);
    transCom = GetComp(TransformCom);
    animationCom = GetComp(AnimationCom);
}

//�z�o�����O
void BaseCharacter_BaseState::Hovering(float elapsedTime)
{
    moveVec = SceneManager::Instance().InputVec(owner->GetGameObject());

    // �󒆂ł̑��x����
    const auto& moveComponent = moveCom.lock(); // moveCom�̃��b�N
    if (moveComponent)
    {
        DirectX::XMFLOAT3 velocity = moveComponent->GetVelocity();

        // �z�o�����O���̏㉺�����̑��x���Ǘ�
        if (velocity.y < 0.05f && HoveringTimer < HoveringTime)
        {
            // �d�͂̉e�����y��
            velocity.y = -GRAVITY_NORMAL * 0.5f * elapsedTime;

            // �z�o�����O�^�C�}�[�����Z
            HoveringTimer += elapsedTime;

            // ���ׂȏ㉺����ǉ��i���V�������o�j
            float hoverOscillation = sin(HoveringTimer * 3.0f) * 0.1f; // �U��0.1�A���g��3.0
            velocity.y += hoverOscillation;
        }
        else if (HoveringTimer >= HoveringTime)
        {
            // �z�o�����O�I����A�ʏ�̗�������
            velocity.y -= GRAVITY_NORMAL * elapsedTime; // �d�͂�K�p
        }

        // �㉺�����̑��x���X�V
        moveComponent->SetVelocity(velocity);

        // �ړ��͂��v�Z
        DirectX::XMFLOAT3 force =
        {
            moveVec.x * moveComponent->GetMoveAcceleration(),
            0.0f, // ���������̂ݗ͂�������
            moveVec.z * moveComponent->GetMoveAcceleration()
        };

        // �ړ��͂𐧌����Ċ��炩����ۂ�
        float maxForce = 10.0f; // �ő�ړ���
        force.x = Mathf::Clamp(force.x, -maxForce, maxForce);
        force.z = Mathf::Clamp(force.z, -maxForce, maxForce);

        // �ړ��͂�K�p
        moveComponent->AddNonMaxSpeedForce(force);
    }
}

auto MakeNormalLowerAnimeParam = [](int index, bool loop)
{
    //�����g�A�j���[�V�����p�\����
    AnimationCom::PlayLowBodyAnimParam param =
    {
        param.lowerAnimaOneId = index,
        param.lowerAnimeTwoId = index,
        param.lowerAnimeThreeId = index,
        param.lowerAnimeFourId = index,
        param.lowerAnimeFiveId = index,
        param.lowerAnimaSixId = index,
        param.lowerAnimaSevenId = index,
        param.lowerAnimaEightId = index,
        param.loop = loop,
        param.rootFlag = false,
        param.blendType = 0,
        param.animeChangeRate = 0.2f,
        param.animeBlendRate = 0.0f
    };

    return param;
};


#pragma region Idle
void BaseCharacter_IdleState::Enter()
{
    //�A�j���[�V��������
    int animaIndex = animationCom.lock()->FindAnimation("Idle");

    //�����g�����Đ�
    animationCom.lock()->SetUpAnimationUpdate(AnimationCom::AnimationType::UpperLowerAnimation);
    animationCom.lock()->PlayLowerBodyOnlyAnimation(MakeNormalLowerAnimeParam(animaIndex, true));
}
void BaseCharacter_IdleState::Execute(const float& elapsedTime)
{
    //�ړ�
    if (owner->IsPushLeftStick())
    {
        ChangeMoveState(CharacterCom::CHARACTER_MOVE_ACTIONS::MOVE);
    }
    //�W�����v
    if (GamePad::BTN_A & owner->GetButtonDown() && moveCom.lock()->OnGround())
    {
        ChangeMoveState(CharacterCom::CHARACTER_MOVE_ACTIONS::JUMP);
    }
}
void BaseCharacter_IdleState::Exit()
{

}
#pragma endregion

#pragma region Move
void BaseCharacter_MoveState::Enter()
{
    //�����A�j���[�V�����Đ��J�n
    animationCom.lock()->SetUpAnimationUpdate(AnimationCom::AnimationType::UpperLowerAnimation);

    AnimationCom::PlayLowBodyAnimParam param =
    {
        param.lowerAnimaOneId = animationCom.lock()->FindAnimation("Walk_Forward"),
        param.lowerAnimeTwoId = animationCom.lock()->FindAnimation("Walk_Back"),
        param.lowerAnimeThreeId = animationCom.lock()->FindAnimation("Walk_Right"),
        param.lowerAnimeFourId = animationCom.lock()->FindAnimation("Walk_Left"),
        param.lowerAnimeFiveId = animationCom.lock()->FindAnimation("Walk_RF"),
        param.lowerAnimaSixId = animationCom.lock()->FindAnimation("Walk_LF"),
        param.lowerAnimaSevenId = animationCom.lock()->FindAnimation("Walk_RB"),
        param.lowerAnimaEightId = animationCom.lock()->FindAnimation("Walk_LB"),
        param.loop = true,
        param.rootFlag = false,
        param.blendType = 2,
        param.animeChangeRate = 0.5f,
        param.animeBlendRate = 0.0f
    };

    animationCom.lock()->PlayLowerBodyOnlyAnimation(param);

    //�����̉��G�t�F�N�g�Đ�
    GameObjectManager::Instance().Find("smokeeffect")->GetComponent<CPUParticle>()->SetActive(true);
}

void BaseCharacter_MoveState::Execute(const float& elapsedTime)
{
    //�ړ�
    MoveInputVec(owner->GetGameObject());

    //�ҋ@
    if (!owner->IsPushLeftStick())
    {
        ChangeMoveState(CharacterCom::CHARACTER_MOVE_ACTIONS::IDLE);
    }
    //�W�����v
    if (GamePad::BTN_A & owner->GetButtonDown() && moveCom.lock()->OnGround())
    {
        ChangeMoveState(CharacterCom::CHARACTER_MOVE_ACTIONS::JUMP);
    }
}

void BaseCharacter_MoveState::Exit()
{
    //�����̉��G�t�F�N�g��~
    GameObjectManager::Instance().Find("smokeeffect")->GetComponent<CPUParticle>()->SetActive(false);
}

#pragma endregion

#pragma region Jump

void BaseCharacter_JumpState::Enter()
{
    //�����ݒ�
    moveCom.lock()->SetAirForce(12.620);

    //�W�����v
    JumpInput(owner->GetGameObject(), 1.5f);

    animationCom.lock()->SetUpAnimationUpdate(AnimationCom::AnimationType::UpperLowerAnimation);
    animationCom.lock()->PlayLowerBodyOnlyAnimation(MakeNormalLowerAnimeParam(animationCom.lock()->FindAnimation("Jump_begin"), false));
}

void BaseCharacter_JumpState::Execute(const float& elapsedTime)
{
    //�z�o�����O
    if (std::strcmp(owner->GetGameObject()->GetName(), "player") == 0)
    {
        Hovering(elapsedTime);
    }

    if (!animationCom.lock()->IsPlayAnimation())
    {
        ChangeMoveState(CharacterCom::CHARACTER_MOVE_ACTIONS::JUMPLOOP);
    }
}
#pragma endregion

#pragma region JumpLoop
void BaseCharacter_JumpLoop::Enter()
{
    //�A�j���[�V��������
    int animaIndex = animationCom.lock()->FindAnimation("Jump_middle");

    //�����g�����Đ�
    animationCom.lock()->SetUpAnimationUpdate(AnimationCom::AnimationType::UpperLowerAnimation);
    animationCom.lock()->PlayLowerBodyOnlyAnimation(MakeNormalLowerAnimeParam(animaIndex, true));
}
void BaseCharacter_JumpLoop::Execute(const float& elapsedTime)
{
    //�z�o�����O
    if (std::strcmp(owner->GetGameObject()->GetName(), "player") == 0)
    {
        Hovering(elapsedTime);
    }

    if (moveCom.lock()->OnGround())
    {
        ChangeMoveState(CharacterCom::CHARACTER_MOVE_ACTIONS::LANDING);
    }
}
#pragma endregion

#pragma region Landing
void BaseCharacter_Landing::Enter()
{
    animationCom.lock()->SetUpAnimationUpdate(AnimationCom::AnimationType::UpperLowerAnimation);
    animationCom.lock()->PlayLowerBodyOnlyAnimation(MakeNormalLowerAnimeParam(animationCom.lock()->FindAnimation("Jump_end"), false));
}
void BaseCharacter_Landing::Execute(const float& elapsedTime)
{
    //�z�o�����O
    if (std::strcmp(owner->GetGameObject()->GetName(), "player") == 0)
    {
        Hovering(elapsedTime);
    }

    //�W�����v
    if (GamePad::BTN_A & owner->GetButtonDown())
    {
        ChangeMoveState(CharacterCom::CHARACTER_MOVE_ACTIONS::JUMP);
    }

    //�A�j���[�V�������I����
    if (!animationCom.lock()->IsPlayAnimation())
    {
        ChangeMoveState(CharacterCom::CHARACTER_MOVE_ACTIONS::IDLE);
    }

    //�ړ�
    if (owner->IsPushLeftStick() && !animationCom.lock()->IsPlayAnimation())
    {
        ChangeMoveState(CharacterCom::CHARACTER_MOVE_ACTIONS::MOVE);
    }
}
void BaseCharacter_Landing::Exit()
{
    HoveringTimer = 0.0f;
}
#pragma endregion

#pragma region Death
void BaseCharacter_DeathState::Enter()
{
    animationCom.lock()->SetUpAnimationUpdate(AnimationCom::AnimationType::NormalAnimation);
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Down"), false);

    //�v���C���[�f��
    owner->GetGameObject()->GetComponent<RendererCom>()->SetDissolveThreshold(0);
    //FPS�p�I�u�W�F�N�g�B��
    GameObjectManager::Instance().Find("armChild")->GetComponent<RendererCom>()->SetDissolveThreshold(1);

    if (std::string(owner->GetGameObject()->GetName()) == "player")
    {
        //�C�x���g�J����
        GameObjectManager::Instance().Find("eventcamera")->GetComponent<CameraCom>()->ActiveCameraChange();
        EventCameraManager::Instance().PlayEventCamera("Data/SerializeData/EventCamera/playerDeath.eventcamera");


        RespawnCom* respawn = GameObjectManager::Instance().Find("respawn")->GetComponent<RespawnCom>().get();
        respawn->SetIsRespawn(true);
    }
}
#pragma endregion

#pragma region Hitscan
void BaseCharacter_HitscanState::Execute(const float& elapsedTime)
{
    auto& ray = owner->GetGameObject()->GetChildFind("rayObj");
    if (ray)
    {
        //���_�̌����Ƀ��C���΂�
        auto& rayCol = ray->GetComponent<RayColliderCom>();
        if (rayCol)
        {
            //�L����
            rayCol->SetEnabled(true);

            DirectX::XMFLOAT3 pos = ray->transform_->GetWorldPosition();

            //���������f����
            DirectX::XMFLOAT3 front;
            int playerNetID = GameObjectManager::Instance().Find("player")->GetComponent<CharacterCom>()->GetNetCharaData().GetNetPlayerID();
            if (playerNetID == charaCom.lock()->GetNetCharaData().GetNetPlayerID())
                front = GameObjectManager::Instance().Find("cameraPostPlayer")->transform_->GetWorldFront();
            else
                front = charaCom.lock()->GetFpsCameraDir();

            rayCol->SetStart(pos);
            rayCol->SetEnd(pos + front * rayLength);
        }
    }

    if (!(CharacterInput::MainAttackButton & owner->GetButton()))
        ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
}

void BaseCharacter_HitscanState::Exit()
{
    //������
    auto& ray = owner->GetGameObject()->GetChildFind("rayObj");
    if (ray)
    {
        auto& rayCol = ray->GetComponent<RayColliderCom>();
        if (rayCol)
            rayCol->SetEnabled(false);
    }
}

void BaseCharacter_HitscanState::ImGui()
{
    ImGui::DragFloat("rayLength", &rayLength);
}

#pragma endregion

#pragma region Capsule
void BaseCharacter_CapsuleState::Execute(const float& elapsedTime)
{
    auto& capsule = owner->GetGameObject()->GetChildFind("capsuleObj");
    if (capsule)
    {
        auto& capsuleCol = capsule->GetComponent<CapsuleColliderCom>();
        if (capsuleCol)
        {
            //�L����
            capsuleCol->SetEnabled(true);

            //���������f����
            DirectX::XMFLOAT3 front;
            int playerNetID = GameObjectManager::Instance().Find("player")->GetComponent<CharacterCom>()->GetNetCharaData().GetNetPlayerID();
            if (playerNetID == charaCom.lock()->GetNetCharaData().GetNetPlayerID())
                front = GameObjectManager::Instance().Find("cameraPostPlayer")->transform_->GetWorldFront();
            else
                front = charaCom.lock()->GetFpsCameraDir();

            capsuleCol->SetPosition1(DirectX::XMFLOAT3(0, 0, 0));
            capsuleCol->SetPosition2(front * capsuleLength);
        }
    }

    //�T�u��胁�C����D��
    if (CharacterInput::MainAttackButton & owner->GetButtonDown())
        ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::MAIN_ATTACK);

    if (!(CharacterInput::SubAttackButton & owner->GetButton()))
        ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
}
void BaseCharacter_CapsuleState::Exit()
{
    //������
    auto& capsule = owner->GetGameObject()->GetChildFind("capsuleObj");
    if (capsule)
    {
        auto& capsuleCol = capsule->GetComponent<CapsuleColliderCom>();
        if (capsuleCol)
            capsuleCol->SetEnabled(false);
    }
}
void BaseCharacter_CapsuleState::ImGui()
{
    ImGui::DragFloat("capsuleLength", &capsuleLength);
}
#pragma endregion

#pragma region StanBall
void BaseCharacter_StanBallState::Enter()
{
    BulletCreate::DamageFire(owner->GetGameObject(), speed, power);
}
void BaseCharacter_StanBallState::Execute(const float& elapsedTime)
{
    ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
}
void BaseCharacter_StanBallState::ImGui()
{
    ImGui::DragFloat("speed", &speed);
    ImGui::DragFloat("power", &power);
}
#pragma endregion

#pragma region KnockbackBall
void BaseCharacter_KnockbackBallState::Enter()
{
    BulletCreate::KnockbackFire(owner->GetGameObject(), speed, power);
}
void BaseCharacter_KnockbackBallState::Execute(const float& elapsedTime)
{
    ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
}
void BaseCharacter_KnockbackBallState::ImGui()
{
    ImGui::DragFloat("speed", &speed);
    ImGui::DragFloat("power", &power);
}
#pragma endregion

#pragma region ULT_ATTACK
void Ult_Attack_State::Enter()
{
    obj = owner->GetGameObject()->GetChildFind("UltAttackChild");
    if (!obj)return;

    //���C�ݒ�
    auto& ray = obj->GetComponent<RayColliderCom>();
    DirectX::XMFLOAT3 start = obj->transform_->GetWorldPosition();

    auto& camera = GameObjectManager::Instance().Find("cameraPostPlayer");
    DirectX::XMFLOAT3 front = camera->transform_->GetWorldFront();
    DirectX::XMFLOAT3 end = start + front * 100;

    //�G�t�F�N�g
    auto& arm = camera->GetChildFind("armChild");
    DirectX::XMFLOAT3 gunPos = {};
    if (arm)
    {
        const auto& model = arm->GetComponent<RendererCom>()->GetModel();
        const auto& node = model->FindNode("gun2");

        gunPos = { node->worldTransform._41,node->worldTransform._42,node->worldTransform._43 };
    }

    //�p�ӂ����G�t�F�N�g�I�u�W�F�N�g�N��
    arm->GetChildFind("attackUltMuzzleEff")->GetComponent<GPUParticle>()->Play();

    //��
    owner->GetGameObject()->GetComponent<AudioCom>()->Stop("P_ATTACKULTSHOOT");
    owner->GetGameObject()->GetComponent<AudioCom>()->Play("P_ATTACKULTSHOOT", false, 10);

    //anim
    auto& armAnim = arm->GetComponent<AnimationCom>();
    armAnim->PlayAnimation(armAnim->FindAnimation("FPS_shoot"), false);
    armAnim->SetAnimationSeconds(0.3f);

    ray->SetStart(start);
    ray->SetEnd(end);
    ray->SetEnabled(true);
}
void Ult_Attack_State::Execute(const float& elapsedTime)
{
    if (!obj)return;

    ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
}
void Ult_Attack_State::Exit()
{
    auto& ray = obj->GetComponent<RayColliderCom>();
    ray->SetEnabled(false);

    obj.reset();
}
#pragma endregion

void BaseCharacter_ReloadState::Enter()
{
    //�A�j���[�V����(�r�����̃A�j���[�V����)
    if (std::string(owner->GetGameObject()->GetName()) == "player")
    {
        auto& arm = GameObjectManager::Instance().Find("armChild");
        auto& armAnim = arm->GetComponent<AnimationCom>();
        armAnim->PlayAnimation(armAnim->FindAnimation("FPS_reload"), false);
    }
    else
    {
        auto& animCom = owner->GetGameObject()->GetComponent<AnimationCom>();
        animCom->PlayUpperBodyOnlyAnimation(animCom->FindAnimation("Dash"), true);
    }
}

void BaseCharacter_ReloadState::Execute(const float& elapsedTime)
{
    AnimationCom* anima = nullptr;
    bool reloadEnd = false;

    if (std::string(owner->GetGameObject()->GetName()) == "player")
    {
        //�A�j���[�V�������I���ΏI���t���O�𗧂Ă�
        auto& arm = GameObjectManager::Instance().Find("armChild");
        anima = arm->GetComponent<AnimationCom>().get();
        reloadEnd = !anima->IsPlayAnimation();
    }
    else
    {
        //�����[�h�A�j���[�V�����Ɠ������Ԃ��ĂΏI���t���O�𗧂Ă�
        reloadTimer += elapsedTime;
        reloadEnd = (reloadTimer >= reloadTime);
    }

    //�I������
    if (reloadEnd)
    {
        ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
        charaCom.lock()->SetMaxBullet();
    }
}

void BaseCharacter_NoneAttack::Enter()
{
    if (std::string(owner->GetGameObject()->GetName()) == "player")
    {
        auto& arm = GameObjectManager::Instance().Find("armChild");
        auto& armAnim = arm->GetComponent<AnimationCom>();
        if (!arm->GetComponent<RendererCom>()->GetModel()) return;
        if (arm->GetComponent<AnimationCom>()->IsPlayAnimation()) return;

        auto& stateMachine = owner->GetMoveStateMachine();

        //���ꂼ��̃X�e�[�g�ɂ��A�j���[�V����(�r�����̃A�j���[�V����)
        PlayStateAnimation(true, stateMachine.GetCurrentState());
    }
    else
    {
        auto& stateMachine = owner->GetMoveStateMachine();
        if (owner->GetGameObject()->GetComponent<AnimationCom>()->IsPlayUpperAnimation()) return;

        //���ꂼ��̃X�e�[�g�ɂ��A�j���[�V����(�r�����̃A�j���[�V����)
        PlayStateAnimation(false, stateMachine.GetCurrentState());
    }
}

void BaseCharacter_NoneAttack::Execute(const float& elapsedTime)
{
    auto& stateMachine = owner->GetMoveStateMachine();

    //���ꂼ��̃X�e�[�g�ɂ��A�j���[�V����(�r�����̃A�j���[�V����)
    if (stateMachine.GetCurrentState() != stateMachine.GetOldState())
    {
        PlayStateAnimation(std::string(owner->GetGameObject()->GetName()) == "player", stateMachine.GetCurrentState());
    }
}

void BaseCharacter_NoneAttack::PlayStateAnimation(bool isPlayer, CharacterCom::CHARACTER_MOVE_ACTIONS state)
{
    AnimationCom* animCom = nullptr;
    GameObj arm;
    if (isPlayer)
    {
        arm = GameObjectManager::Instance().Find("armChild");
        animCom = arm->GetComponent<AnimationCom>().get();
    }
    else
    {
        animCom = owner->GetGameObject()->GetComponent<AnimationCom>().get();
    }
    float animeSpeed = 0;

    switch (state)
    {
    case CharacterCom::CHARACTER_MOVE_ACTIONS::IDLE:

        isPlayer ?
            animCom->PlayAnimation(animCom->FindAnimation("FPS_idol"), true) :
            animCom->PlayUpperBodyOnlyAnimation(animCom->FindAnimation("Idle"), true);
        break;

    case CharacterCom::CHARACTER_MOVE_ACTIONS::MOVE:

        if (isPlayer)
        {
            animCom->PlayAnimation(animCom->FindAnimation("FPS_walk"), true);

            //�A�j���[�V�����X�s�[�h�ύX
            animeSpeed = owner->GetGameObject()->GetComponent<MovementCom>()->GetMoveMaxSpeed() - owner->GetGameObject()->GetComponent<MovementCom>()->GetFisrtMoveMaxSpeed();
            if (animeSpeed < 0)animeSpeed = 0;

            arm->GetComponent<RendererCom>()->GetModel()->GetResource()->GetAnimationsEdit()[animCom->FindAnimation("FPS_walk")].animationspeed
                = 1 + animeSpeed * 0.1f;
        }
        else
        {
            if (owner->GetDashFlag())
            {
                animCom->PlayUpperBodyOnlyAnimation(animCom->FindAnimation("Dash"), true);
            }
            else
            {
                animCom->PlayUpperBodyOnlyAnimation(animCom->FindAnimation("Walk_Forward"), true);
            }
        }

        break;

    case CharacterCom::CHARACTER_MOVE_ACTIONS::JUMP:
        isPlayer ?
            animCom->PlayAnimation(animCom->FindAnimation("FPS_Jump_begin"), false) :
            animCom->PlayUpperBodyOnlyAnimation(animCom->FindAnimation("Jump_begin"), false);
        break;

    case CharacterCom::CHARACTER_MOVE_ACTIONS::JUMPLOOP:
        isPlayer ?
            animCom->PlayAnimation(animCom->FindAnimation("FPS_Jump_middle"), false) :
            animCom->PlayUpperBodyOnlyAnimation(animCom->FindAnimation("Jump_middle"), false);
        break;

    case CharacterCom::CHARACTER_MOVE_ACTIONS::LANDING:
        isPlayer ?
            animCom->PlayAnimation(animCom->FindAnimation("FPS_Jump_end"), false) :
            animCom->PlayUpperBodyOnlyAnimation(animCom->FindAnimation("Jump_end"), false);        break;
        break;

    default:
        break;
    }
}
