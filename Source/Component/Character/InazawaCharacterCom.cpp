#include "InazawaCharacterCom.h"
#include "StateMachine\Behaviar\InazawaCharacterState.h"
#include "StateMachine\Behaviar\BaseCharacterState.h"
#include "Scene/SceneManager.h"
#include "Component\Camera\CameraCom.h"
#include "Component\Renderer\RendererCom.h"
#include "CharaStatusCom.h"
#include "Component\UI\PlayerUI.h"
#include "Component\Sprite\Sprite.h"
#include "Component\Particle\GPUParticle.h"
#include "Component\Collsion\ColliderCom.h"
#include "Component\Character\RemoveTimerCom.h"
#include "Component\Audio\AudioCom.h"

void InazawaCharacterCom::Start()
{
    //�X�e�[�g�o�^
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::IDLE, std::make_shared<BaseCharacter_IdleState>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::MOVE, std::make_shared<BaseCharacter_MoveState>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::JUMP, std::make_shared<BaseCharacter_JumpState>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::JUMPLOOP, std::make_shared<BaseCharacter_JumpLoop>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::LANDING, std::make_shared<BaseCharacter_Landing>(this));
    moveStateMachine.AddState(CHARACTER_MOVE_ACTIONS::DEATH, std::make_shared<BaseCharacter_DeathState>(this));

    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::MAIN_ATTACK, std::make_shared<InazawaCharacter_AttackState>(this));
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::SUB_SKILL, std::make_shared<InazawaCharacter_ESkillState>(this));
    //�E���g�ǉ�
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::ULT, std::make_shared<Ult_Attack_State>(this));
    attackStateMachine.AddState(CHARACTER_ATTACK_ACTIONS::NONE, std::make_shared<BaseCharacter_NoneAttack>(this));

    moveStateMachine.ChangeState(CHARACTER_MOVE_ACTIONS::IDLE);
    attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::NONE);
}

void InazawaCharacterCom::Update(float elapsedTime)
{
    CharacterCom::Update(elapsedTime);

    FPSArmAnimation();

    shootTimer += elapsedTime;
    //�U����s����
    if (attackInputSave)
    {
        if (shootTimer >= shootTime)
        {
            //�X�L���������̓��^�[��
            if (attackStateMachine.GetCurrentState() != CHARACTER_ATTACK_ACTIONS::SUB_SKILL)
            {
                attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::MAIN_ATTACK);
            }
            attackInputSave = false;
        }
        if (CharacterInput::MainAttackButton & GetButtonUp())
            attackInputSave = false;
    }

    //�E���g�G�t�F�N�g
    if (attackUltRayObj.lock())
    {
        auto& rayCol = attackUltRayObj.lock()->GetComponent<Collider>();
        if (rayCol)
        {
            for (auto& obj : rayCol->OnHitGameObject())
            {
                {
                    std::shared_ptr<GameObject> attackUltEffBomb = GameObjectManager::Instance().Create();
                    attackUltEffBomb->SetName("attackUltEffBomb");
                    attackUltEffBomb->transform_->SetWorldPosition(obj.hitPos);
                    std::shared_ptr<GPUParticle> eff = attackUltEffBomb->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/attackUltBombCircle.gpuparticle", 300);
                    eff->Play();
                    attackUltEffBomb->AddComponent<RemoveTimerCom>(3);
                    {
                        std::shared_ptr<GameObject> attackUltEffBomb02 = attackUltEffBomb->AddChildObject();
                        attackUltEffBomb02->SetName("attackUltEffBomb02");
                        std::shared_ptr<GPUParticle> eff = attackUltEffBomb02->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/attackUltBombCircleLight.gpuparticle", 100);
                        eff->Play();
                    }
                    {
                        std::shared_ptr<GameObject> attackUltEffBomb03 = attackUltEffBomb->AddChildObject();
                        attackUltEffBomb03->SetName("attackUltEffBomb03");
                        std::shared_ptr<GPUParticle> eff = attackUltEffBomb03->AddComponent<GPUParticle>("Data/SerializeData/GPUEffect/attackUltBombFire.gpuparticle", 50);
                        eff->Play();
                    }

                    //��
                    GetGameObject()->GetComponent<AudioCom>()->Stop("P_ATTACK_ULT_BOOM");
                    GetGameObject()->GetComponent<AudioCom>()->Play("P_ATTACK_ULT_BOOM", false, 10);
                }
            }
        }
    }
}

void InazawaCharacterCom::OnGUI()
{
    CharacterCom::OnGUI();
    ImGui::DragFloat("shootTime", &shootTime);
    ImGui::DragFloat("shootTimer", &shootTimer);

    ImGui::DragInt("attackUltCountMax", &attackUltCountMax);
    ImGui::DragInt("attackUltCounter", &attackUltCounter);
}

void InazawaCharacterCom::MainAttackDown()
{
    //�X�L���������̓��^�[��
    if (attackStateMachine.GetCurrentState() == CHARACTER_ATTACK_ACTIONS::SUB_SKILL)return;

    ////���C�A�j���[�V�����̏ꍇ�̓��^�[��
    //auto& arm = GetGameObject()->GetChildFind("cameraPostPlayer")->GetChildFind("armChild");
    //auto& armAnim = arm->GetComponent<AnimationCom>();
    //if (armAnim->GetCurrentAnimationIndex() == armAnim->FindAnimation("FPS_shoot"))
    //{
    //    attackInputSave = true; //��s���͕ۑ�
    //    return;
    //}

    //�E���g������
    if (UseUlt())
    {
        attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::ULT);

        attackUltCounter++;
        if (attackUltCounter >= attackUltCountMax)
        {
            //�G�t�F�N�g�؂�
            GameObjectManager::Instance().Find("attackUltSide1")->GetComponent<GPUParticle>()->SetLoop(false);
            GameObjectManager::Instance().Find("attackUltSide2")->GetComponent<GPUParticle>()->SetLoop(false);
            FinishUlt();
        }

        return;
    }

    if (shootTimer < shootTime)
    {
        attackInputSave = true; //��s���͕ۑ�
        return;
    }

    //�A�^�b�N
    attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::MAIN_ATTACK);
    attackInputSave = false;
}

//�u�����N
void InazawaCharacterCom::SubAttackDown()
{
    //���͒l�擾
    DirectX::XMFLOAT3 moveVec = SceneManager::Instance().InputVec(GetGameObject());

    //�_�b�V��
    auto& moveCom = GetGameObject()->GetComponent<MovementCom>();
    DirectX::XMFLOAT3 v = moveVec * 60.0f;
    moveCom->AddNonMaxSpeedForce(v);
}

void InazawaCharacterCom::SubSkill()
{
    if (!UseUlt())
        attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::SUB_SKILL);
    else
        ResetESkillCool();
}

void InazawaCharacterCom::UltSkill()
{
    //int counter = *GetRCounter();
    //if (counter >= 0 && counter <= 4)
    //{
    //    std::string coreName = "core" + std::to_string(4 - counter);
    //    const auto& sprite = GameObjectManager::Instance().Find(coreName.c_str())->GetComponent<Sprite>();
    //    if (sprite)
    //    {
    //        sprite->EasingPlay();
    //    }
    //}

    attackUltCounter = 0;

    //�G�t�F�N�g�N��
    GameObjectManager::Instance().Find("attackUltSide1")->GetComponent<GPUParticle>()->SetLoop(true);
    GameObjectManager::Instance().Find("attackUltSide2")->GetComponent<GPUParticle>()->SetLoop(true);
    //�X�e�[�g��������
    attackStateMachine.ChangeState(CHARACTER_ATTACK_ACTIONS::NONE);

}

//�I����Ă܂��B���̃X�e�[�g�}�V�������������Ȃ������́H(�s�v�c�ł������Ȃ�)by���
void InazawaCharacterCom::FPSArmAnimation()
{
    if (std::string(GetGameObject()->GetName()) != "player")return;
    auto& arm = GetGameObject()->GetChildFind("cameraPostPlayer")->GetChildFind("armChild");
    auto& armAnim = arm->GetComponent<AnimationCom>();

    //�ҋ@
    if (moveStateMachine.GetCurrentState() == CHARACTER_MOVE_ACTIONS::IDLE)
    {
        if (armAnim->GetCurrentAnimationIndex() == armAnim->FindAnimation("FPS_idol"))return;

        if (armAnim->GetCurrentAnimationIndex() != armAnim->FindAnimation("FPS_shoot"))
            armAnim->PlayAnimation(armAnim->FindAnimation("FPS_idol"), true);
    }

    //�ړ�
    if (moveStateMachine.GetCurrentState() == CHARACTER_MOVE_ACTIONS::MOVE)
    {
        if (armAnim->GetCurrentAnimationIndex() != armAnim->FindAnimation("FPS_walk"))
        {
            if (armAnim->GetCurrentAnimationIndex() == armAnim->FindAnimation("FPS_shoot"))
            {
                if (armAnim->IsEventCalling("attackEnd"))
                    armAnim->PlayAnimation(armAnim->FindAnimation("FPS_walk"), true);
            }
            else
                armAnim->PlayAnimation(armAnim->FindAnimation("FPS_walk"), true);
        }
    }

    //�A�j���[�V�����X�s�[�h�ύX
    float fmax = GetGameObject()->GetComponent<MovementCom>()->GetFisrtMoveMaxSpeed();
    float max = GetGameObject()->GetComponent<MovementCom>()->GetMoveMaxSpeed();

    float v = max - fmax;
    if (v < 0)v = 0;

    arm->GetComponent<RendererCom>()->GetModel()->GetResource()->GetAnimationsEdit()[armAnim->FindAnimation("FPS_walk")].animationspeed
        = 1 + v * 0.1f;
}