#include "JankratCharacterState.h"
#include "Component\Phsix\RigidBodyCom.h"

JankratCharacter_BaseState::JankratCharacter_BaseState(CharacterCom* owner) : State(owner)
{
    //‰ŠúÝ’è
    charaCom = GetComp(JankratCharacterCom);
    moveCom = GetComp(MovementCom);
    transCom = GetComp(TransformCom);
    animationCom = GetComp(AnimationCom);
}

void JankratCharacter_MainAtkState::Enter()
{
    GameObj bullet = GameObjectManager::Instance().Create();
    bullet->transform_->SetScale({ 0.01f,0.01f,0.01f });
    DirectX::XMFLOAT3 pos = transCom.lock()->GetWorldPosition();
    bullet->transform_->SetWorldPosition({ pos.x,pos.y + 2,pos.z });
    RigidBodyCom* rigid = bullet->AddComponent<RigidBodyCom>(false, RigidBodyCom::RigidType::PrimitiveSphere).get();
    
    rigid->SetNormalizeScale(100);
    RendererCom* r = bullet->AddComponent<RendererCom>(SHADER_ID_MODEL::STAGEDEFERRED, BLENDSTATE::MULTIPLERENDERTARGETS, DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE::SOLID_CULL_BACK, true, false).get();
    r->LoadModel("Data/Model/Ball/SplitBall.mdl");

    charaCom.lock()->SetHaveBullet(bullet);
}

void JankratCharacter_MainAtkState::Execute(const float& elapsedTime)
{
    if (charaCom.lock()->GetHaveBullet())
    {
        RigidBodyCom* rigid = charaCom.lock()->GetHaveBullet()->GetComponent<RigidBodyCom>().get();
        
        //‹…‚ªƒZƒbƒg‚³‚ê‚Ä‚¢‚½‚ç”­ŽË
        rigid->SetMass(0.1f);
        rigid->SetRestitution(2.0f);
        rigid->SetRigidFlag(physx::PxRigidBodyFlag::eENABLE_CCD, true);

        DirectX::XMFLOAT3 vec = owner->GetGameObject()->transform_->GetWorldFront();
        rigid->AddForce(Mathf::Normalize({vec.x, vec.y + 0.2f, vec. z}) * 1);
    }

    ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
}
