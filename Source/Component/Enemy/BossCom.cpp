#include "BossCom.h"
#include "Math/Mathf.h"
#include "Graphics/Graphics.h"
#include "Component/Renderer/RendererCom.h"
#include "Component\System\TransformCom.h"
#include <Component\MoveSystem\MovementCom.h>

//初期設定
void BossCom::Start()
{
    //ステート登録
    state.AddState(BossState::IDLE, std::make_shared<Boss_IdleState>(this));
    state.AddState(BossState::IDLESTOP, std::make_shared<Boss_IdleStopState>(this));
    state.AddState(BossState::MOVE, std::make_shared<Boss_MoveState>(this));
    state.AddState(BossState::SHORTATTACK1, std::make_shared<Boss_SA1>(this));
    state.AddState(BossState::SHORTATTACK2, std::make_shared<Boss_SA2>(this));
    state.AddState(BossState::LARIATSTART, std::make_shared<Boss_LARIATSTART>(this));
    state.AddState(BossState::LARIATLOOP, std::make_shared<Boss_LARIATLOOP>(this));
    state.AddState(BossState::LARIATEND, std::make_shared<Boss_LARIATEND>(this));
    //state.AddState(BossState::JUMPATTACKSTART, std::make_shared<Boss_JumpAttackStart>(this));
    //state.AddState(BossState::JUMPATTACKEND, std::make_shared<Boss_JumpAttackEnd>(this));
    state.AddState(BossState::SHOTSTART, std::make_shared<Boss_ShotStart>(this));
    state.AddState(BossState::SHOTLOOP, std::make_shared<Boss_ShotCharge>(this));
    state.AddState(BossState::SHOTEND, std::make_shared<Boss_Shot>(this));
    state.AddState(BossState::UPSHOTSTART, std::make_shared<Boss_UpShotStart>(this));
    state.AddState(BossState::UPSHOTCHARGE, std::make_shared<Boss_UpShotCharge>(this));
    state.AddState(BossState::UPSHOTLOOP, std::make_shared<Boss_UpShotLoop>(this));
    state.AddState(BossState::UPSHOTEND, std::make_shared<Boss_UpShotEnd>(this));

    //無敵時間
    GetGameObject()->GetComponent<CharaStatusCom>()->SetInvincibleTime(0.1f);

    //初期ステート登録
    state.ChangeState(BossState::IDLE);
}

//更新処理
void BossCom::Update(float elapsedTime)
{
    //ステート更新処理
    state.Update(elapsedTime);

    //ターゲット位置は常にプレイヤー
    targetposition = GameObjectManager::Instance().Find("player")->transform_->GetWorldPosition();

    //後ろにいる場合旋回する
    BackTurn();

    //DebugPrimitive
    Graphics::Instance().GetDebugRenderer()->DrawCylinder(GetGameObject()->transform_->GetWorldPosition(), GetGameObject()->transform_->GetWorldPosition(), meleerange, 0.1f, { 1.0f,0.0f,0.0f,1.0f });
    Graphics::Instance().GetDebugRenderer()->DrawCylinder(GetGameObject()->transform_->GetWorldPosition(), GetGameObject()->transform_->GetWorldPosition(), longrange, 0.1f, { 0.0f,1.0f,0.0f,1.0f });
    Graphics::Instance().GetDebugRenderer()->DrawCylinder(GetGameObject()->transform_->GetWorldPosition(), GetGameObject()->transform_->GetWorldPosition(), walkrange, 0.1f, { 0.0f,0.0f,1.0f,1.0f });
}

//imgui
void BossCom::OnGUI()
{
    ImGui::DragFloat("meleerange", &meleerange, 1.0f, 0.0f, 50.0f);
    ImGui::DragFloat("longrange", &longrange, 1.0f, 0.0f, 50.0f);
    ImGui::DragFloat("walkrange", &walkrange, 1.0f, 0.0f, 50.0f);

    state.ImGui();
}

//索敵関数
bool BossCom::Search(float range)
{
    DirectX::XMFLOAT3 playerpos = GameObjectManager::Instance().Find("player")->transform_->GetWorldPosition();

    // プレイヤーとの高低差を考慮して3Dで距離判定をする
    float vx = playerpos.x - GetGameObject()->transform_->GetWorldPosition().x;
    float vy = playerpos.y - GetGameObject()->transform_->GetWorldPosition().y;
    float vz = playerpos.z - GetGameObject()->transform_->GetWorldPosition().z;
    float dist = sqrtf(vx * vx + vy * vy + vz * vz);

    if (dist < range)
    {
        // XZ平面での距離を計算
        float distXZ = sqrtf(vx * vx + vz * vz);

        // 単位ベクトル化
        vx /= distXZ;
        vz /= distXZ;

        // 方向ベクトル化
        float frontX = sinf(GetGameObject()->transform_->GetEulerRotation().y);
        float frontZ = cosf(GetGameObject()->transform_->GetEulerRotation().y);

        // 2つのベクトルの内積値で前後判定
        float dot = (frontX * vx) + (frontZ * vz);
        if (dot > 0.0f)
        {
            return true;
        }
        if (dot < 0.0f)
        {
            return true;
        }
    }

    return false;
}

// 目標地点へ移動
void BossCom::MoveToTarget(float movespeed, float turnspeed)
{
    // ターゲット方向への進行ベクトル
    DirectX::XMFLOAT2 Tvec = TargetVec();

    // 移動処理
    DirectX::XMFLOAT3 vec = { Tvec.x, 0.0f, Tvec.y };
    GetGameObject()->GetComponent<MovementCom>()->AddForce(vec * movespeed);

    // 回転処理
    GetGameObject()->transform_->Turn(vec, turnspeed);
}

//ジャンプ
void BossCom::Jump(float power)
{
    // ターゲット方向への進行ベクトル
    DirectX::XMFLOAT2 vec = TargetVec();

    // ジャンプ力を設定
    DirectX::XMFLOAT3 jumppower = { vec.x, power, vec.y };
    GetGameObject()->GetComponent<MovementCom>()->AddForce(jumppower);
}

//後ろにいる場合旋回する
void BossCom::BackTurn()
{
    // プレイヤーの位置取得
    DirectX::XMFLOAT3 playerpos = GameObjectManager::Instance().Find("player")->transform_->GetWorldPosition();
    DirectX::XMFLOAT3 bossPos = GetGameObject()->transform_->GetWorldPosition();

    // プレイヤーとボス間の距離ベクトル
    float vx = playerpos.x - bossPos.x;
    float vy = playerpos.y - bossPos.y;
    float vz = playerpos.z - bossPos.z;

    // XZ平面での距離を計算
    float distXZ = sqrtf(vx * vx + vz * vz);

    // プレイヤー方向の単位ベクトル
    vx /= distXZ;
    vz /= distXZ;

    // ボスの前方向ベクトル (Y軸回転角から算出)
    float rotationY = DirectX::XMConvertToRadians(GetGameObject()->transform_->GetEulerRotation().y);
    float frontX = sinf(rotationY);
    float frontZ = cosf(rotationY);

    // 内積で前後判定
    float dot = (frontX * vx) + (frontZ * vz);

    if (dot > 0.0f)
    {
        // ボスを回転
        MoveToTarget(0.0f, 0.001f);  // ここで、回転速度を適切に設定
    }
    if (dot < 0.0f)
    {
        // ボスを回転
        MoveToTarget(0.0f, 0.03f);  // ここで、回転速度を適切に設定
    }
}

// ターゲット方向への進行ベクトルを算出
DirectX::XMFLOAT2 BossCom::TargetVec()
{
    DirectX::XMFLOAT2 vec;
    vec.x = targetposition.x - GetGameObject()->transform_->GetWorldPosition().x;
    vec.y = targetposition.z - GetGameObject()->transform_->GetWorldPosition().z;
    float dist = sqrtf(vec.x * vec.x + vec.y * vec.y);
    vec.x /= dist;
    vec.y /= dist;

    return vec;
}