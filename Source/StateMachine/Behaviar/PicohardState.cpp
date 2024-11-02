#include "PicohardState.h"
#include "Component/Collsion/ColliderCom.h"
#include "Component/Renderer/RendererCom.h"
#include "Component/Camera/CameraCom.h"
#include "Netwark/Photon/StaticSendDataManager.h"

Picohard_BaseState::Picohard_BaseState(CharacterCom* owner) : State(owner)
{
    //初期設定
    charaCom = GetComp(CharacterCom);
    moveCom = GetComp(MovementCom);
    transCom = GetComp(TransformCom);
    animationCom = GetComp(AnimationCom);
}

void Picohard_LeftClick::Enter()
{
    auto& hammerZikuX = owner->GetGameObject()->GetChildFind("hammerZikuX");
    auto& hammerZikuY = hammerZikuX->GetChildFind("hammerZikuY");

    hammerZikuY->transform_->SetEulerRotation({ 0, rangeAngle, 0 });
    angle = rangeAngle;

    auto& hammer = hammerZikuY->GetChildFind("hammer");
    hammer->GetComponent<Collider>()->SetEnabled(true);
}

void Picohard_LeftClick::Execute(const float& elapsedTime)
{
    auto& hammerZikuX = owner->GetGameObject()->GetChildFind("hammerZikuX");
    float x = GameObjectManager::Instance().Find("cameraPostPlayer")->transform_->GetEulerRotation().x;
    hammerZikuX->transform_->SetEulerRotation({ x,0,0 });

    auto& hammerZikuY = hammerZikuX->GetChildFind("hammerZikuY");

    angle -= elapsedTime * speed;
    if (angle < -rangeAngle)angle = rangeAngle;

    hammerZikuY->transform_->SetEulerRotation({ 0, angle, 0 });

    //サブ（盾）を優先
    if (CharacterInput::SubAttackButton & owner->GetButtonDown())
        ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::SUB_ATTACK);

    if (!(CharacterInput::MainAttackButton & owner->GetButton()))
        ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
}

void Picohard_LeftClick::Exit()
{
    auto& hammerZikuX = owner->GetGameObject()->GetChildFind("hammerZikuX");
    auto& hammerZikuY = hammerZikuX->GetChildFind("hammerZikuY");
    hammerZikuY->transform_->SetEulerRotation({ 0, 0, 0 });

    auto& hammer = hammerZikuY->GetChildFind("hammer");
    hammer->GetComponent<Collider>()->SetEnabled(false);
}

void Picohard_RightClick::Enter()
{
    auto& sieldZiku = owner->GetGameObject()->GetChildFind("sieldZikuX");
    auto& sield = sieldZiku->GetChildFind("sield");

    sield->GetComponent<Collider>()->SetEnabled(true);
    sield->GetComponent<RendererCom>()->SetEnabled(true);
}

void Picohard_RightClick::Execute(const float& elapsedTime)
{
    auto& sieldZiku = owner->GetGameObject()->GetChildFind("sieldZikuX");
    float x = GameObjectManager::Instance().Find("cameraPostPlayer")->transform_->GetEulerRotation().x;
    sieldZiku->transform_->SetEulerRotation({ x,0,0 });

    if (!(CharacterInput::SubAttackButton & owner->GetButton()))
    {
        auto& sield = sieldZiku->GetChildFind("sield");
        sield->GetComponent<Collider>()->SetEnabled(false);
        sield->GetComponent<RendererCom>()->SetEnabled(false);

        ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
    }
}

void Picohard_RightClick::Exit()
{
}

void Picohard_LeftShift::Enter()
{
    //仮ジャンプアニメ
    animationCom.lock()->SetUpAnimationUpdate(AnimationCom::AnimationType::NormalAnimation);
    animationCom.lock()->PlayAnimation(animationCom.lock()->FindAnimation("Jump_Enter"), true);

    //当たり判定起動
    std::shared_ptr<GameObject> chargeCol = owner->GetGameObject()->GetChildFind("chargeCol");
    chargeCol->GetComponent<Collider>()->SetEnabled(true);

    owner->SetMoveFlag(false);
    timer = 0;

    hitID = -1;
}

void Picohard_LeftShift::Execute(const float& elapsedTime)
{
    std::shared_ptr<GameObject> myObj = owner->GetGameObject();

    //移動
    {
        std::shared_ptr<GameObject> cameraObj = SceneManager::Instance().GetActiveCamera();
        CameraCom* cameraCom = cameraObj->GetComponent<CameraCom>().get();

        const DirectX::XMFLOAT3& forwardVec = cameraCom->GetFront();

        myObj->GetComponent<MovementCom>()->AddForce(forwardVec * moveSpeed);
    }

    //回転
    {
        // 入力情報を取得
        GamePad& gamePad = Input::Instance().GetGamePad();

        //ステックのXY取得
        float ax = gamePad.GetAxisLX();

        DirectX::XMFLOAT3 euler = myObj->transform_->GetEulerRotation();
        euler.y += ax * angleSpeed * elapsedTime;
        myObj->transform_->SetEulerRotation(euler);
    }

    //当たり判定処理
    std::shared_ptr<GameObject> chargeCol = owner->GetGameObject()->GetChildFind("chargeCol");
    if (hitID < 0)
    {
        auto& hit = chargeCol->GetComponent<Collider>()->OnHitGameObject();
        for (auto& h : hit)
        {
            auto& hitChara = h.gameObject.lock()->GetComponent<CharacterCom>();
            if (!hitChara)continue;

            hitID = hitChara->GetNetID();
        }
    }
    //ヒット時処理
    else
    {
        //スタン
        StaticSendDataManager::Instance().SetSendStan(owner->GetNetID(), hitID, 1);

        //相手移動処理
        DirectX::XMFLOAT3 enemyPos = chargeCol->transform_->GetWorldPosition();
        StaticSendDataManager::Instance().SetSendMovePos(owner->GetNetID(), hitID, enemyPos);
    }

    timer += elapsedTime;
    if (timer > 1.5f && (CharacterInput::LeftShiftButton & owner->GetButton()))
    {
        ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
    }
    if (timer > time)
    {
        ChangeAttackState(CharacterCom::CHARACTER_ATTACK_ACTIONS::NONE);
    }
}

void Picohard_LeftShift::Exit()
{
    owner->SetMoveFlag(true);

    //当たり判定停止
    std::shared_ptr<GameObject> chargeCol = owner->GetGameObject()->GetChildFind("chargeCol");
    chargeCol->GetComponent<Collider>()->SetEnabled(false);
}

void Picohard_LeftShift::ImGui()
{
    ImGui::DragFloat("moveSpeed", &moveSpeed, 0.1f);
    ImGui::DragFloat("angleSpeed", &angleSpeed, 0.1f);
}