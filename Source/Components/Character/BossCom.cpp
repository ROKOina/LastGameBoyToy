#include "BossCom.h"
#include "GameSource/Math/Mathf.h"
#include <random>

//初期設定
void BossCom::Start()
{
    //ステート登録
    state.AddState(BossState::IDLE, std::make_shared<Boss_IdleState>(this));
    state.AddState(BossState::MOVE, std::make_shared<Boss_MoveState>(this));
    state.AddState(BossState::JUMP, std::make_shared <Boss_JumpState>(this));
    state.AddState(BossState::JUMPLOOP, std::make_shared <Boss_JumpLoopState>(this));
    state.AddState(BossState::LANDINGATTACK, std::make_shared <Boss_LandingState>(this));
    state.AddState(BossState::ATTACK, std::make_shared <Boss_AttackState>(this));

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
}

//imgui
void BossCom::OnGUI()
{
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
    // ターゲット方向への進行ベクトルを算出
    DirectX::XMFLOAT2 Tvec = TargetVec();

    //移動、旋回処理
    DirectX::XMFLOAT3 vec = { Tvec.x,0.0f,Tvec.y };
    GetGameObject()->GetComponent<MovementCom>()->AddForce(vec * movespeed);
    GetGameObject()->transform_->Turn(vec, turnspeed);
}

//乱数計算
int BossCom::ComputeRandom()
{
    if (availableNumbers.empty())
    {
        // 全ての数字が一度出た場合、リストをリセット
        availableNumbers = { 1, 2, 3 };
    }

    // 乱数生成エンジン (Mersenne Twister)
    std::random_device rd;  // 非決定的な乱数生成
    std::mt19937 gen(rd()); // メルセンヌ・ツイスタのシードに乱数を使う
    std::uniform_int_distribution<int> dis(0, availableNumbers.size() - 1);

    int index = dis(gen);  // ランダムにインデックスを生成
    int randomValue = availableNumbers[index];

    // 使用済みの値をリストから削除
    availableNumbers.erase(availableNumbers.begin() + index);

    return randomValue;  // ランダムな値を返す
}

//ジャンプ
void BossCom::Jump(float power)
{
    // ターゲット方向への進行ベクトルを算出
    DirectX::XMFLOAT2 vec = TargetVec();

    DirectX::XMFLOAT3 jumppower = { vec.x,power,vec.y };
    GetGameObject()->GetComponent<MovementCom>()->AddForce(jumppower);
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