#include "PlayerUI.h"
#include "Math\Mathf.h"
#include "Component\Character\CharacterCom.h"

UI_Skill::UI_Skill(const char* filename, SpriteShader spriteshader, bool collsion, float min, float max) :UiSystem(filename, spriteshader, collsion)
{
    changePosValue = min - max;
    //もしマイナスなら整数値に変える
    if (changePosValue <= 0.0f) {
        changePosValue *= -1.0f;
    }
    originalPos.y = min;
}

void UI_Skill::Start()
{
    this->UiSystem::Start();
}

void UI_Skill::Update(float elapsedTime)
{
    //ゲージの倍率を求める
    valueRate = *variableValue / maxValue;
    float addPos = changePosValue * valueRate;
    if (!isDebug) {
        spc.position = { spc.position.x,originalPos.y - addPos };
    }
}

UI_BoosGauge::UI_BoosGauge(int num)
{
    player = GameObjectManager::Instance().Find("player");
    for (int i = 0; i < num; i++) {
        //ブースト回数だけUIを増やす
        //<Frame>//
        std::shared_ptr<GameObject> BoostFrame = GameObjectManager::Instance().Create();
        std::string name = "BoostFrame_";
        std::string number = std::to_string(i);
        name += number;
        std::shared_ptr<UiSystem> frame = BoostFrame->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/BoostFrame.ui", Sprite::SpriteShader::DEFALT, false);
        BoostFrame->SetName(name.c_str());
        //位置調整
        frame->spc.position = { frame->spc.position.x + (i * (frame->spc.texSize.x * frame->spc.scale.x)) ,frame->spc.position.y - i * 19.0f };

        //<Gauge//
        std::shared_ptr<GameObject> BoostGauge = GameObjectManager::Instance().Create();
        name = "BoostGauge_";
        name += number;
        BoostGauge->SetName(name.c_str());
        std::shared_ptr<UiSystem> gauge = BoostGauge->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/BoostGauge.ui", Sprite::SpriteShader::DEFALT, false);
        gauge->SetVariableValue(player.lock()->GetComponent<CharacterCom>()->GetDashGauge());
        //位置調整
        gauge->spc.position = { gauge->spc.position.x + (i * (gauge->spc.texSize.x * gauge->spc.scale.x)) ,gauge->spc.position.y - i * 19.0f };

        originlTexSize = gauge->spc.texSize;
        frames.emplace_back(BoostFrame);
        gauges.emplace_back(BoostGauge);
    }

    this->num = num;
    maxDashGauge = player.lock()->GetComponent<CharacterCom>()->GetDahsGaugeMax();
    value = player.lock()->GetComponent<CharacterCom>()->GetDashGauge();

    //区切りの値を求める
    separateValue = maxDashGauge / num;
}

void UI_BoosGauge::Start()
{
    //なぜかinitializeでは親子付けできなかった
    //親子付け
    for (int i = 0; i < num; i++) {
        this->GetGameObject()->AddChildObject(frames.at(i));
        this->GetGameObject()->AddChildObject(gauges.at(i));
    }
}

void UI_BoosGauge::Update(float elapsedTime)
{
    std::shared_ptr<UiSystem> gauge = gauges.at(0).get()->GetComponent<UiSystem>();
    std::shared_ptr<UiSystem> gauge2 = gauges.at(1).get()->GetComponent<UiSystem>();

    // 変化値がマイナスに行かないように補正
    *value = Mathf::Clamp(*value, 0.01f, maxDashGauge);
    float valueRate;
    //ゲージの倍率を求める
    valueRate = *value / (maxDashGauge);

    for (int i = 0; i < num; i++)
    {
        std::shared_ptr<UiSystem> gaugeSegment = gauges.at(i).get()->GetComponent<UiSystem>();
        float segmentStart = separateValue * i;       // 現在のゲージの開始位置
        float segmentEnd = separateValue * (i + 1);   // 現在のゲージの終了位置

        if (*value > segmentEnd) {
            // セグメントがフルの場合
            gaugeSegment->spc.texSize = { originlTexSize.x, gaugeSegment->spc.texSize.y };
        }
        else if (*value > segmentStart) {
            // セグメントが部分的に埋まっている場合
            float partialRate = (*value - segmentStart) / separateValue; // セグメント内の比率
            gaugeSegment->spc.texSize = { originlTexSize.x * partialRate, gaugeSegment->spc.texSize.y };
        }
        else {
            // セグメントが空の場合
            gaugeSegment->spc.texSize = { 0, gaugeSegment->spc.texSize.y };
        }
    }
}

UI_LockOn::UI_LockOn(int num)
{
    for (int i = 0; i < num; i++) {
        //リアクターを保持
        std::string name = "Reactar";
        name = name + std::to_string(i);
        reacters.emplace_back(GameObjectManager::Instance().Find(name.c_str()));
        similarity.emplace_back(0.0f);
    }
    //カメラ保持
    camera = GameObjectManager::Instance().Find("cameraPostPlayer");
    //Uiのゲームオブジェクト生成
    lockOn = GameObjectManager::Instance().Create();
    lockOn->SetName("LockOn");
    lockOnUi = lockOn->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/LockOnBase.ui", Sprite::SpriteShader::DEFALT, false);
    lockOnUi->spc.objectname = "";
    lockOnUi->spc.color.w = 0.0f;

    gauge = GameObjectManager::Instance().Create();
    gauge->SetName("LockOnGauge");
    gaugeUi = gauge->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/LockOnGaugeFrame.ui", Sprite::SpriteShader::DEFALT, false);
    gaugeUi->spc.objectname = "";
    gaugeUi->spc.color.w = 0.0f;
}

void UI_LockOn::Start()
{
    this->GetGameObject()->AddChildObject(lockOn);
    this->GetGameObject()->AddChildObject(gauge);
}

void UI_LockOn::Update(float elapsedTime)
{
    std::shared_ptr<GameObject> nearObj = SearchObjct();
    if (nearObj != nullptr) {
        gaugeUi->spc.objectname = nearObj->GetName();
        lockOnUi->spc.objectname = nearObj->GetName();
        LockIn(elapsedTime);
    }
    else {
        // gaugeUi->spc.objectname = "";
        // lockOnUi->spc.objectname = "";
        LockOut(elapsedTime);
    }
}

std::shared_ptr<GameObject> UI_LockOn::SearchObjct()
{
    //カメラのポジション
    DirectX::XMFLOAT3 cameraPos = camera.lock()->GetComponent<CameraCom>()->GetEye();

    //カメラのベクトル
    DirectX::XMFLOAT3 cameraVec = camera.lock()->GetComponent<CameraCom>()->GetFocus() - cameraPos;
    cameraVec = Mathf::Normalize(cameraVec);
    //どの範囲まで描画するかの閾値
    float threshold = 0.99f;

    std::shared_ptr<GameObject> nearReacter;
    float maxSimilarity = 0.0f;
    int i = 0;
    for (auto& reacter : reacters) {
        DirectX::XMFLOAT3 reacterPos = reacter->GetComponent<TransformCom>()->GetWorldPosition();
        DirectX::XMFLOAT3 reacterDirection = reacterPos - cameraPos;
        reacterDirection = Mathf::Normalize(reacterDirection);

        //1に近いほど視線が合っている
        similarity.at(i) = Mathf::Dot(cameraVec, reacterDirection);

        //閾値を超えているかつ今いる中で一番近い時
        if (similarity.at(i) > threshold && similarity.at(i) > maxSimilarity) {
            maxSimilarity = similarity.at(i);
            nearReacter = reacter;
        }
        i++;
    }

    if (nearReacter) {
        return nearReacter;
    }
    else {
        return nullptr;
    }
}

void UI_LockOn::LockIn(float elapsedTime)
{
    float changeValue = 1.3f;
    if (gaugeUi->spc.color.w <= 1.0f) {
        gaugeUi->spc.color.w += 6.0f * elapsedTime;
        lockOnUi->spc.color.w += 6.0f * elapsedTime;
    }
    if (gaugeUi->spc.scale.x > 0.3f) {
        gaugeUi->spc.scale.x -= changeValue * elapsedTime;
        gaugeUi->spc.scale.y -= changeValue * elapsedTime;
        lockOnUi->spc.scale.x -= changeValue * elapsedTime;
        lockOnUi->spc.scale.y -= changeValue * elapsedTime;
    }
}

void UI_LockOn::LockOut(float elapsedTime)
{
    float changeValue = 1.7f;
    if (gaugeUi->spc.color.w >= 0.0f) {
        gaugeUi->spc.color.w -= 6.0f * elapsedTime;
        lockOnUi->spc.color.w -= 6.0f * elapsedTime;
    }
    if (gaugeUi->spc.scale.x <= 0.4f) {
        gaugeUi->spc.scale.x += changeValue * elapsedTime;
        gaugeUi->spc.scale.y += changeValue * elapsedTime;
        lockOnUi->spc.scale.x += changeValue * elapsedTime;
        lockOnUi->spc.scale.y += changeValue * elapsedTime;
    }
}

void UI_LockOn::OnGUI()
{
    ImGui::Text(lockOnUi->spc.objectname.c_str());
    for (int i = 0; i < 4; i++) {
        ImGui::DragFloat("near", &similarity.at(i));
    }
}