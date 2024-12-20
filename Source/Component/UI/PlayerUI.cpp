#include "PlayerUI.h"
#include "Math\Mathf.h"
#include "Component\Character\CharacterCom.h"
#include "Component\Character\CharaStatusCom.h"
#include "Component\Character\InazawaCharacterCom.h"
#include"StateMachine\Behaviar\InazawaCharacterState.h"

UI_Skill::UI_Skill(const char* filename, SpriteShader spriteshader, bool collsion, float min, float max) :UiSystem(filename, spriteshader, collsion)
{
    changePosValue = min - max;
    //もしマイナスなら整数値に変える
    if (changePosValue <= 0.0f) {
        changePosValue *= -1.0f;
    }    originalPos.y = min;
    maxPos.y = max;
}

void UI_Skill::Start()
{
    this->UiSystem::Start();
}

void UI_Skill::Update(float elapsedTime)
{
    // 変化値がマイナスに行かないように補正
    *variableValue = Mathf::Clamp(*variableValue, 0.01f, maxValue);
    //ゲージの倍率を求める
    valueRate = *variableValue / maxValue;
    float addPos = changePosValue * valueRate;
    if (!isDebug) {
        GetGameObject()->transform_->SetLocalPosition({ 0,0,0 });
        GetGameObject()->transform_->SetWorldPosition({ GetGameObject()->transform_->GetWorldPosition().x ,originalPos.y - addPos,0 });
    }
    this->UiSystem::Update(elapsedTime);
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

UI_LockOn::UI_LockOn(int num, float min, float max)
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
       //1番外の枠
    lockOn = GameObjectManager::Instance().Create();
    lockOn->SetName("LockOn");
    lockOnUi = lockOn->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/LockOnBase.ui", Sprite::SpriteShader::DEFALT, false);
    lockOnUi->spc.objectname = "";
    lockOnUi->spc.color.w = 0.0f;

    //2番目の枠
    lockOn2 = GameObjectManager::Instance().Create();
    lockOn2->SetName("LockOn2");
    lockOn2Ui = lockOn2->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/LockOnBase2.ui", Sprite::SpriteShader::DEFALT, false);
    lockOn2Ui->spc.objectname = "";
    lockOn2Ui->spc.color.w = 0.0f;

    //ゲージの外枠
    gaugeFrame = GameObjectManager::Instance().Create();
    gaugeFrame->SetName("LockOnGaugeFrame");
    gaugeFrameUi = gaugeFrame->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/LockOnGaugeFrame.ui", Sprite::SpriteShader::DEFALT, false);
    gaugeFrameUi->spc.objectname = "";
    gaugeFrameUi->spc.color.w = 0.0f;

    //ゲージのマスク
    gaugeMask = GameObjectManager::Instance().Create();
    gaugeMask->SetName("LockOnGaugeMask");
    gaugeMaskUi = gaugeMask->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/LockOnGaugeMask.ui", Sprite::SpriteShader::DEFALT, false);
    gaugeMaskUi->spc.objectname = "";
    gaugeMaskUi->spc.color.w = 0.0f;

    //ゲージ本体
    gauge = GameObjectManager::Instance().Create();
    gauge->SetName("LockOnGauge");
    gaugeUi = gauge->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/LockOnGauge.ui", Sprite::SpriteShader::DEFALT, false);
    gaugeUi->spc.objectname = "";
    gaugeUi->spc.color.w = 0.0f;

    minAngle = min;
    maxAngle = max;
}

void UI_LockOn::Start()
{
    this->GetGameObject()->AddChildObject(lockOn);
    this->GetGameObject()->AddChildObject(lockOn2);
    this->GetGameObject()->AddChildObject(gaugeFrame);
    this->GetGameObject()->AddChildObject(gaugeMask);
    this->GetGameObject()->AddChildObject(gauge);
}

void UI_LockOn::Update(float elapsedTime)
{
    std::shared_ptr<GameObject> nearObj = SearchObjct();
    if (nearObj != nullptr) {
        gaugeFrameUi->spc.objectname = nearObj->GetName();
        gaugeUi->spc.objectname = nearObj->GetName();
        gaugeMaskUi->spc.objectname = nearObj->GetName();
        lockOnUi->spc.objectname = nearObj->GetName();
        lockOn2Ui->spc.objectname = nearObj->GetName();
        UpdateGauge(elapsedTime, nearObj);
        LockIn(elapsedTime);
    }
    else {
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

void UI_LockOn::UpdateGauge(float elapsedTime, std::shared_ptr<GameObject> obj)
{
    float hp = *obj->GetComponent<CharaStatusCom>()->GetHitPoint();
    float maxHp = obj->GetComponent<CharaStatusCom>()->GetMaxHitpoint();
    // 変化値がマイナスに行かないように補正
    hp = Mathf::Clamp(hp, 0.01f, maxHp);

    //ゲージの倍率を求める
    float  valueRate = hp / maxHp;
    valueRate = 1 - valueRate;
    float addAngle = maxAngle * valueRate;

    gaugeMaskUi->spc.angle = minAngle + addAngle;
}

void UI_LockOn::LockIn(float elapsedTime)
{
    float changeValue = 5.0f;
    //透明度
    if (gaugeFrameUi->spc.color.w <= 1.0f) {
        gaugeFrameUi->spc.color.w += 6.0f * elapsedTime;
        gaugeUi->spc.color.w += 6.0f * elapsedTime;
        gaugeMaskUi->spc.color.w += 6.0f * elapsedTime;
        lockOnUi->spc.color.w += 6.0f * elapsedTime;
        lockOn2Ui->spc.color.w += 6.0f * elapsedTime;
    }

    //スケール
    if (gaugeFrameUi->spc.scale.x > 0.5f) {
        gaugeFrameUi->spc.scale.x -= changeValue * elapsedTime;
        gaugeFrameUi->spc.scale.y -= changeValue * elapsedTime;

        gaugeMaskUi->spc.scale.x -= changeValue * elapsedTime;
        gaugeMaskUi->spc.scale.y -= changeValue * elapsedTime;

        gaugeUi->spc.scale.x -= changeValue * elapsedTime;
        gaugeUi->spc.scale.y -= changeValue * elapsedTime;

        lockOnUi->spc.scale.x -= changeValue * elapsedTime;
        lockOnUi->spc.scale.y -= changeValue * elapsedTime;

        lockOn2Ui->spc.scale.x -= changeValue * elapsedTime;
        lockOn2Ui->spc.scale.y -= changeValue * elapsedTime;
    }

    //回転
    if (lockOn2Ui->spc.angle < 90) {
        lockOn2Ui->spc.angle += 5;
    }
    else {
        lockOn2Ui->spc.angle += 1;
    }
}

void UI_LockOn::LockOut(float elapsedTime)
{
    float changeValue = 1.7f;
    if (gaugeFrameUi->spc.color.w >= 0.0f) {
        gaugeFrameUi->spc.color.w -= 6.0f * elapsedTime;
        lockOnUi->spc.color.w -= 6.0f * elapsedTime;
        lockOn2Ui->spc.color.w -= 6.0f * elapsedTime;

        gaugeUi->spc.color.w -= 6.0f * elapsedTime;
        gaugeMaskUi->spc.color.w -= 6.0f * elapsedTime;
    }
    if (gaugeFrameUi->spc.scale.x <= 1.0f) {
        gaugeFrameUi->spc.scale.x += changeValue * elapsedTime;
        gaugeFrameUi->spc.scale.y += changeValue * elapsedTime;

        gaugeUi->spc.scale.x += changeValue * elapsedTime;
        gaugeUi->spc.scale.y += changeValue * elapsedTime;

        gaugeMaskUi->spc.scale.x += changeValue * elapsedTime;
        gaugeMaskUi->spc.scale.y += changeValue * elapsedTime;

        lockOnUi->spc.scale.x += changeValue * elapsedTime;
        lockOnUi->spc.scale.y += changeValue * elapsedTime;

        lockOn2Ui->spc.scale.x += changeValue * elapsedTime;
        lockOn2Ui->spc.scale.y += changeValue * elapsedTime;
    }

    //回転
    if (lockOn2Ui->spc.angle > 0) {
        lockOn2Ui->spc.angle -= 15;
    }
}

void UI_LockOn::OnGUI()
{
    ImGui::Text(lockOnUi->spc.objectname.c_str());
    for (int i = 0; i < 4; i++) {
        ImGui::DragFloat("near", &similarity.at(i));
    }
}

UI_E_SkillCount::UI_E_SkillCount(int num)
{
    float centerX = 960;//中央値
    for (int i = 0; i < num; i++) {
        SkillCore localCore;
        float offset = (i - (num - 1) / 2.0f) * spacing; //配置用のoffset
        //外枠のゲームオブジェクト生成
        std::shared_ptr<GameObject> coreFrame = GameObjectManager::Instance().Create();;

        std::string name = "coreFrame";
        std::string number = std::to_string(i);
        name += number;
        coreFrame->SetName(name.c_str());
        localCore.coreFrameUi = coreFrame->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/E_SkillCoreFrame.ui", Sprite::SpriteShader::DEFALT, false);
        localCore.coreFrameUi->spc.position = { centerX + offset,localCore.coreFrameUi->spc.position.y };
        coreFrames.emplace_back(coreFrame);

        //本体のゲームオブジェクト生成
        std::shared_ptr<GameObject> core;
        core = GameObjectManager::Instance().Create();
        name = "core";
        number = std::to_string(i);
        name += number;
        core->SetName(name.c_str());
        localCore.coreUi = core->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/E_SkillCore.ui", Sprite::SpriteShader::DEFALT, false);
        localCore.coreUi->spc.position = { centerX + offset,localCore.coreUi->spc.position.y };
        cores.emplace_back(core);
        coresUi.emplace_back(localCore);
    }

    gaugeFrame = GameObjectManager::Instance().Create();
    std::string name = "skillGauegFrame";
    gaugeFrame->SetName(name.c_str());
    gaugeFrameUi = gaugeFrame->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/E_SkillGaugeFrame.ui", Sprite::SpriteShader::DEFALT, false);

    gauge = GameObjectManager::Instance().Create();
    name = "skillGaueg";
    gauge->SetName(name.c_str());
    gaugeUi = gauge->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/E_SkillGauge.ui", Sprite::SpriteShader::DEFALT, false);
    originalTexSize = gaugeUi->spc.texSize;

    this->num = num;
}

void UI_E_SkillCount::Start()
{
    //親子付け
    for (int i = 0; i < num; i++) {
        this->GetGameObject()->AddChildObject(coreFrames.at(i));
        this->GetGameObject()->AddChildObject((cores.at(i)));
    }
    this->GetGameObject()->AddChildObject(gaugeFrame);
    this->GetGameObject()->AddChildObject(gauge);
    //各パラメーター設定

    player = GameObjectManager::Instance().Find("player");
    if (player.lock()->GetComponent<CharacterCom>()->GetAttackStateMachine().GetState<InazawaCharacter_ESkillState>() != nullptr)
    {
        arrowCount = &player.lock()->GetComponent<CharacterCom>()->GetAttackStateMachine().GetState<InazawaCharacter_ESkillState>()->arrowCount;
        skillTimer = &player.lock()->GetComponent<CharacterCom>()->GetAttackStateMachine().GetState<InazawaCharacter_ESkillState>()->skillTimer;
        skillTime = player.lock()->GetComponent<CharacterCom>()->GetAttackStateMachine().GetState<InazawaCharacter_ESkillState>()->skillTime;
        isShot = &player.lock()->GetComponent<CharacterCom>()->GetAttackStateMachine().GetState<InazawaCharacter_ESkillState>()->isShot;
    }
}

void UI_E_SkillCount::Update(float elapsedTime)
{
    //ごめん消しましたby上野
    //if (player.lock()->GetComponent<CharacterCom>()->GetAttackStateMachine().GetCurrentState() == CharacterCom::CHARACTER_ATTACK_ACTIONS::SUB_SKILL) {
    //    UpdateCore(elapsedTime);
    //    UpdateGauge(elapsedTime);
    //}
    //else {
    //    for (int                                                                               i = 0; i < num; i++) {
    //        coresUi.at(i).coreFrameUi->spc.color.w                                             = 0.0f;
    //        coresUi.at(i).coreUi->spc.color.w                                                  = 0.0f;
    //    }
    //    gaugeUi->spc.color.w                                                                   = 0.0f;
    //    gaugeFrameUi->spc.color.w                                                              = 0.0f;
    //}
}

void UI_E_SkillCount::UpdateGauge(float elapsedTime)
{
    // 変化値がマイナスに行かないように補正
    *skillTimer = Mathf::Clamp(*skillTimer, 0.01f, skillTime);
    //ゲージの倍率を求める
    float valueRate = *skillTimer / skillTime;

    gaugeUi->spc.color.w = 1.0f;
    gaugeUi->spc.texSize = { originalTexSize.x * valueRate,gaugeUi->spc.texSize.y };
}

void UI_E_SkillCount::UpdateCore(float elapsedTime)
{
    for (int i = 0; i < num; i++) {
        if (i < *arrowCount) {
            coresUi.at(i).coreFrameUi->spc.color.w = 1.0f;
            coresUi.at(i).coreUi->spc.color.w = 1.0f;
        }
        else {
            coresUi.at(i).coreUi->spc.color.w = 0.0f;
        }
    }
    gaugeFrameUi->spc.color.w = 1.0f;
}

void UI_E_SkillCount::OnGUI()
{
    ImGui::DragFloat("spcaisn", &spacing);
}

UI_Ult_Count::UI_Ult_Count(int num)
{
    float centerX = 960;//中央値
    for (int i = 0; i < num; i++) {
        SkillCore localCore;
        float offset = (i - (num - 1) / 2.0f) * spacing; //配置用のoffset
        //外枠のゲームオブジェクト生成
        std::shared_ptr<GameObject> coreFrame = GameObjectManager::Instance().Create();;

        std::string name = "coreFrame";
        std::string number = std::to_string(i);
        name += number;
        coreFrame->SetName(name.c_str());
        localCore.coreFrameUi = coreFrame->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/E_SkillCoreFrame.ui", Sprite::SpriteShader::DEFALT, false);
        localCore.coreFrameUi->spc.position = { centerX + offset,localCore.coreFrameUi->spc.position.y };
        coreFrames.emplace_back(coreFrame);

        //本体のゲームオブジェクト生成
        std::shared_ptr<GameObject> core;
        core = GameObjectManager::Instance().Create();
        name = "core";
        number = std::to_string(i);
        name += number;
        core->SetName(name.c_str());
        localCore.coreUi = core->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/UltCore.ui", Sprite::SpriteShader::DEFALT, false);
        localCore.coreUi->spc.position = { centerX + offset,localCore.coreUi->spc.position.y };
        cores.emplace_back(core);
        coresUi.emplace_back(localCore);
    }

    this->num = num;
}

void UI_Ult_Count::Start()
{
    //親子付け
    for (int i = 0; i < num; i++) {
        this->GetGameObject()->AddChildObject(coreFrames.at(i));
        this->GetGameObject()->AddChildObject((cores.at(i)));
    }
    //各パラメーター設定
    player = GameObjectManager::Instance().Find("player");
    ultCount = player.lock()->GetComponent<InazawaCharacterCom>()->GetRCounter();
}

void UI_Ult_Count::Update(float elapsedTime)
{
    //例外処理クソ過ぎ
    if (player.lock()->GetComponent<InazawaCharacterCom>() != nullptr)
    {
        if (player.lock()->GetComponent<InazawaCharacterCom>()->UseUlt())
        {
            UpdateCore(elapsedTime);
        }
        else
        {
            for (int i = 0; i < num; i++)
            {
                coresUi.at(i).coreFrameUi->spc.color.w = 0.0f;
                coresUi.at(i).coreUi->spc.color.w = 0.0f;
            }
        }
    }
}

void UI_Ult_Count::UpdateCore(float elapsedTime)
{
    for (int i = 0; i < num; i++) {
        if (i < num - *ultCount) {
            coresUi.at(i).coreFrameUi->spc.color.w = 1.0f;
            coresUi.at(i).coreUi->spc.color.w = 1.0f;
        }
        else {
            coresUi.at(i).coreUi->spc.color.w = 0.0f;
        }
    }
}

void PlayerUIManager::Register()
{
    ////共通のUI////
        //キャンバス
    auto& canvas = GameObjectManager::Instance().Create();
    canvas->SetName("Canvas");
    USE_SKILL use_skill[2];
    use_skill[0] = USE_SKILL::E;
    use_skill[1] = USE_SKILL::RIGHT_CLICK;
    int count = 0;
    if (player.expired())return;
    for (int i = 0; i < sizeof(use_skill) / sizeof(USE_SKILL); i++) {
        if (player.lock()->GetComponent<CharacterCom>()->GetUseSkill() == use_skill[i]) {
            CreateSkillUI(use_skill[i], count);
            count++;
        }
    }

    //レティクル
    CreateReticleUI();
    //ULT
    CreateUltUI();
    //Hp
    CreateHpUI();
    //Boost
    CreateBoostUI();
    ////////////////////////////////

    //キャラ固有のUI
    switch (player.lock()->GetComponent<CharacterCom>()->GetNetCharaData().GetCharaID())
    {
    case (int)RegisterChara::CHARA_LIST::INAZAWA:

        break;

    case (int)RegisterChara::CHARA_LIST::FARAH:
        break;

    case (int)RegisterChara::CHARA_LIST::JANKRAT:
        break;

    case (int)RegisterChara::CHARA_LIST::SOLIDER:
        break;
    }
}

void PlayerUIManager::UIUpdate(float elapsedTime)
{
    if (!bookingRegister) return;

    std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");
    if (canvas) {
        GameObjectManager::Instance().Remove(canvas);
    }
    else {
        Register();
        bookingRegister = false;
    }
}

void PlayerUIManager::CreateSkillUI(USE_SKILL use_skill, int count)
{
    std::string name = "Data/Texture/PlayerUI/" + (std::string)player.lock()->GetComponent<CharacterCom>()->GetName() + "/Skill/";
    std::string iconName = "Data/Texture/KeyBoard/";
    CharacterCom::SkillCoolID skillNum;
    //位置をずらす定数
    const float offset = 120.0f;
    switch (use_skill)
    {
    case Q:
        skillNum = CharacterCom::SkillCoolID::Q;
        break;
    case E:
        name += "skill_E.png";
        iconName += "keyboard_e_outline.png";
        skillNum = CharacterCom::SkillCoolID::E;
        break;
    case RIGHT_CLICK:
        name += "skill_Q.png";
        iconName += "mouse_right_outline.png";
        skillNum = CharacterCom::SkillCoolID::LeftClick;
        break;
    default:
        break;
    }

    std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");

    //SkillFrame2
    std::shared_ptr<GameObject> skillFrame = canvas->AddChildObject();
    skillFrame->SetName("Skill_Frame");
    auto& a = skillFrame->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/SkillFrame1_02.ui", Sprite::SpriteShader::DEFALT, false);
    a->spc.position = { a->spc.position.x - (count * offset),a->spc.position.y };

    //SkillMask
    {
        std::shared_ptr<GameObject> skillGaueHide = skillFrame->AddChildObject();
        skillGaueHide->SetName("SkillGaugeHide");
        auto& a = skillGaueHide->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/SkillGaugeMask.ui", Sprite::SpriteShader::DEFALT, false);
        a->spc.position = { a->spc.position.x - (count * offset),a->spc.position.y };
    }

    //SkillGauge
    {
        std::shared_ptr<GameObject> skillGauge = skillFrame->AddChildObject();
        skillGauge->SetName("SkillGauge");
        std::shared_ptr<UI_Skill>skillGaugeCmp = skillGauge->AddComponent<UI_Skill>("Data/SerializeData/UIData/Player/SkillGauge1.ui", Sprite::SpriteShader::DEFALT, false, 997, 908);
        std::shared_ptr<GameObject>player = GameObjectManager::Instance().Find("player");
        skillGaugeCmp->SetMaxValue(player->GetComponent<CharacterCom>()->GetSkillCoolTime(skillNum));
        float* i = player->GetComponent<CharacterCom>()->GetSkillCoolTimerPointer(skillNum);
        skillGaugeCmp->SetVariableValue(i);
        skillGaugeCmp->spc.position = { skillGaugeCmp->spc.position.x - (count * offset),skillGaugeCmp->spc.position.y };
    }

    //SkillFrame
    {
        std::shared_ptr<GameObject> skillFrame2 = skillFrame->AddChildObject();
        skillFrame2->SetName("SkillFrame2");
        auto& a = skillFrame2->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/SkillFrame1_01.ui", Sprite::SpriteShader::DEFALT, false);
        skillFrame2->transform_->SetWorldPosition({ skillFrame2->transform_->GetWorldPosition().x - (count * offset),skillFrame2->transform_->GetWorldPosition().y,0 });
        a->spc.position = { a->spc.position.x - (count * offset),a->spc.position.y };
    }

    //Skill_E
    {
        std::shared_ptr<GameObject> skillIcon = skillFrame->AddChildObject();
        skillIcon->SetName("Skill_E");
        auto& a = skillIcon->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/Skill_E.ui", Sprite::SpriteShader::DEFALT, false);
        skillIcon->GetComponent<UiSystem>()->LoadTexture(name);
        a->spc.position = { a->spc.position.x - (count * offset),a->spc.position.y };
    }

    //KeyBoardIcon
    {
        std::shared_ptr<GameObject> skillIcon = skillFrame->AddChildObject();
        skillIcon->SetName("KeyIcon");
        auto& a = skillIcon->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/KeyIcon.ui", Sprite::SpriteShader::DEFALT, false);
        skillIcon->GetComponent<UiSystem>()->LoadTexture(iconName);
        a->spc.position = { a->spc.position.x - (count * offset),a->spc.position.y };
    }
}

void PlayerUIManager::CreateReticleUI()
{
    //ロードするテクスチャを設定
    std::string name = "Data/Texture/PlayerUI/" + (std::string)player.lock()->GetComponent<CharacterCom>()->GetName() + "/Sight.png";
    std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");
    std::shared_ptr<GameObject> reticle = canvas->AddChildObject();
    auto& a = reticle->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/Reticle.ui", Sprite::SpriteShader::DEFALT, false);
    a->LoadTexture(name);
}

void PlayerUIManager::CreateUltUI()
{
    //ロードするテクスチャを設定(アイコンができてから)
    std::string name = "Data/Texture/PlayerUI/" + (std::string)player.lock()->GetComponent<CharacterCom>()->GetName() + "/UltIcon.png";

    //UltFrame
    {
        std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");
        std::shared_ptr<GameObject> hpMemori = canvas->AddChildObject();
        hpMemori->SetName("UltFrame");
        std::shared_ptr<UiSystem> fade = hpMemori->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/UltFrame.ui", Sprite::SpriteShader::DEFALT, false);
    }

    //UltHideGauge
    {
        std::shared_ptr<GameObject> ultFrame = GameObjectManager::Instance().Find("UltFrame");
        std::shared_ptr<GameObject> ultHideGauge = ultFrame->AddChildObject();
        ultHideGauge->SetName("UltHideGauge");
        ultHideGauge->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/UltHideGauge.ui", Sprite::SpriteShader::DEFALT, false);
    }

    //UltGauge
    {
        std::shared_ptr<GameObject> ultFrame = GameObjectManager::Instance().Find("UltFrame");
        std::shared_ptr<GameObject> ultGauge = ultFrame->AddChildObject();
        ultGauge->SetName("UltGauge");

        std::shared_ptr<UI_Skill>ultGaugeCmp = ultGauge->AddComponent<UI_Skill>("Data/SerializeData/UIData/Player/UltGauge.ui", Sprite::SpriteShader::DEFALT, false, 1084, 890);
        std::shared_ptr<GameObject>player = GameObjectManager::Instance().Find("player");
        ultGaugeCmp->SetMaxValue(player->GetComponent<CharacterCom>()->GetUltGaugeMax());
        float* i = player->GetComponent<CharacterCom>()->GetUltGauge();
        ultGaugeCmp->SetVariableValue(i);
    }
}

void PlayerUIManager::CreateHpUI()
{
    //ロードするテクスチャを設定(アイコンができてから)
    std::string name = "Data/Texture/PlayerUI/" + (std::string)player.lock()->GetComponent<CharacterCom>()->GetName() + "/CharaIcon.png";

    //HpFrame
    {
        std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");
        std::shared_ptr<GameObject> hpFrame = canvas->AddChildObject();
        hpFrame->SetName("HpFrame");
        hpFrame->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/HpFrame.ui", Sprite::SpriteShader::DEFALT, false);
    }
    //HpGauge
    {
        std::shared_ptr<GameObject> hpFrame = GameObjectManager::Instance().Find("HpFrame");
        std::shared_ptr<GameObject> hpGauge = hpFrame->AddChildObject();
        hpGauge->SetName("HpGauge");
        std::shared_ptr<UiGauge>gauge = hpGauge->AddComponent<UiGauge>("Data/SerializeData/UIData/Player/HpGauge.ui", Sprite::SpriteShader::DEFALT, true, UiSystem::X_ONLY_ADD);
        gauge->SetMaxValue(GameObjectManager::Instance().Find("player")->GetComponent<CharaStatusCom>()->GetMaxHitpoint());
        float* i = GameObjectManager::Instance().Find("player")->GetComponent<CharaStatusCom>()->GetHitPoint();
        gauge->SetVariableValue(i);
    }
}

void PlayerUIManager::CreateBoostUI()
{
    int boostCount = player.lock()->GetComponent<CharacterCom>()->GetDahsGaugeMax() / 5;
    //Boost
    {
        std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");
        std::shared_ptr<GameObject> hpMemori = canvas->AddChildObject();
        hpMemori->SetName("boostGauge2");

        hpMemori->AddComponent<UI_BoosGauge>(boostCount);
    }
}

void PlayerUIManager::BookingRegistrationUI(std::shared_ptr<GameObject> obj)
{
    player = obj;
    bookingRegister = true;
}