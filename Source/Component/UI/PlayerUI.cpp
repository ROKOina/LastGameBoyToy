#include "PlayerUI.h"
#include "Math\Mathf.h"
#include "Component\Character\CharacterCom.h"
#include "Component\Character\CharaStatusCom.h"
#include "Component\Character\InazawaCharacterCom.h"
#include"StateMachine\Behaviar\InazawaCharacterState.h"
#include "Component\Collsion\ColliderCom.h"
#include "Component\UI\UiFlag.h"
#include "Component\UI\Font.h"
#include "Scene\ScenePVP\ScenePVP.h"
#include "Netwark/Photon/StaticSendDataManager.h"

// UTF-8 (std::string) → UTF-16 (std::wstring) 変換
std::wstring UTF8ToWString3(const std::string& str) {
    std::wstring result;
    size_t i = 0;
    while (i < str.size()) {
        unsigned char c = str[i];
        if (c <= 0x7F) {
            result.push_back(c);
            ++i;
        }
        else if ((c & 0xE0) == 0xC0) {
            wchar_t wc = ((c & 0x1F) << 6) | (str[i + 1] & 0x3F);
            result.push_back(wc);
            i += 2;
        }
        else if ((c & 0xF0) == 0xE0) {
            wchar_t wc = ((c & 0x0F) << 12) | ((str[i + 1] & 0x3F) << 6) | (str[i + 2] & 0x3F);
            result.push_back(wc);
            i += 3;
        }
        else if ((c & 0xF8) == 0xF0) {
            wchar_t wc = ((c & 0x07) << 18) | ((str[i + 1] & 0x3F) << 12) | ((str[i + 2] & 0x3F) << 6) | (str[i + 3] & 0x3F);
            result.push_back(wc);
            i += 4;
        }
        else {
            // 不正なUTF-8データを無視する
            ++i;
        }
    }
    return result;
}

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

UI_PlayerHpUI::UI_PlayerHpUI()
{
    player = GameObjectManager::Instance().Find("player");
    //HpFrame
    {
        std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");
        std::shared_ptr<GameObject> hpFrame = canvas->AddChildObject();
        hpFrame->SetName("HpFrame");
        hpFrame->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/HpFrame.ui", Sprite::SpriteShader::DEFALT, false);
    }

    //currenthp
    {
        std::shared_ptr<GameObject> hpFrame = GameObjectManager::Instance().Find("HpFrame");
        std::shared_ptr<GameObject> hpnum = hpFrame->AddChildObject();
        hpnum->SetName("HpNum");
        std::shared_ptr<UI_HpNum>hn = hpnum->AddComponent<UI_HpNum>();
    }

    //hpslashu
    {
        std::shared_ptr<GameObject> hpFrame = GameObjectManager::Instance().Find("HpFrame");
        std::shared_ptr<GameObject> hps = hpFrame->AddChildObject();
        hps->SetName("slashu");
        hps->AddComponent<Sprite>("Data/SerializeData/UIData/Player/slashu.ui", Sprite::SpriteShader::DEFALT, false);
    }

    //maxhp
    {
        std::shared_ptr<GameObject> hpFrame = GameObjectManager::Instance().Find("HpFrame");
        std::shared_ptr<GameObject> hpnum = hpFrame->AddChildObject();
        auto& playerchara = player.lock()->GetComponent<CharaStatusCom>();
        hpnum->SetName("MaxHp");
        float maxhp = playerchara->GetMaxHitpoint();  //最大HP
        std::wstring numstr = std::to_wstring(static_cast<int>(maxhp));
        std::shared_ptr<Font> font = hpnum->AddComponent<Font>("Data/Texture/Font/BitmapFont.font", 1024);
        font->scale = 0.75f;
        font->color = { 1,1,1,0.5f };
        font->str = numstr;  //L付けてね
        font->position = { 252.0f,860.0f };
    }

    //electro
    {
        std::shared_ptr<GameObject> hpFrame = GameObjectManager::Instance().Find("HpFrame");
        std::shared_ptr<GameObject> electro = hpFrame->AddChildObject();
        electro->SetName("electro");
        std::shared_ptr<Sprite>e = electro->AddComponent<Sprite>("Data/SerializeData/UIData/Player/electro.ui", Sprite::SpriteShader::DEFALT, false);
        e->SetColumns(6);
        e->SetRows(2);
        e->SetFrameRate(20.2f);
    }

    //HpGauge
    {
        std::shared_ptr<GameObject> hpFrame = GameObjectManager::Instance().Find("HpFrame");
        std::shared_ptr<GameObject> hpGauge = hpFrame->AddChildObject();
        hpGauge->SetName("HpGauge");
        std::shared_ptr<UiGauge>gauge = hpGauge->AddComponent<UiGauge>("Data/SerializeData/UIData/Player/HpGauge.ui", Sprite::SpriteShader::DEFALT, true, UiSystem::X_ONLY_ADD);
        gauge->SetMaxValue(player.lock()->GetComponent<CharaStatusCom>()->GetMaxHitpoint());
        hp = player.lock()->GetComponent<CharaStatusCom>()->GetHitPoint();
        gauge->SetVariableValue(hp);
    }
}

void UI_PlayerHpUI::Start()
{
}

void UI_PlayerHpUI::Update(float elapsedTime)
{
    std::shared_ptr<UiGauge> gauge = GameObjectManager::Instance().Find("HpGauge")->GetComponent<UiGauge>();
    std::shared_ptr<Sprite> electro = GameObjectManager::Instance().Find("electro")->GetComponent<Sprite>();
    if (*hp <= 100) {
        gauge->spc.color = { 0.96f,0.945f,0.242f,1 };
        electro->spc.color = { 0.96f,0.945f,0.242f,1 };

        if (*hp <= 50) {
            gauge->spc.color = { 0.876f, 0.090f, 0.090f, 1.000f };
            electro->spc.color = { 0.827f, 0.105f, 0.105f, 1.000f };
        }
    }
    else {
    }
}

UI_HPEffect::UI_HPEffect(const char* filename, SpriteShader spriteshader, bool collsion, int gaugeTexSize, std::weak_ptr<GameObject> obj, int num) :UiSystem(filename, spriteshader, collsion)
{
    this->gaugeTexSize = gaugeTexSize;
    //ゲージ本体のテクスチャサイズを10で割った数字を保持
    divideTexSize = gaugeTexSize / 10;
    character = obj;
    maxHp = character.lock()->GetComponent<CharaStatusCom>()->GetMaxHitpoint();
    memoryId = num;
    spc.color = { spc.color.x,spc.color.y,spc.color.z,0 };
}

void UI_HPEffect::Start()
{
    this->UiSystem::Start();
}

void UI_HPEffect::Update(float elapsedTime)
{
    valueRate = *character.lock()->GetComponent<CharaStatusCom>()->GetHitPoint() / maxHp;
    DirectX::XMFLOAT3 localPosition = this->GetGameObject()->GetComponent<TransformCom>()->GetLocalPosition();
    DirectX::XMFLOAT3 worldPosition = this->GetGameObject()->GetComponent<TransformCom>()->GetWorldPosition();
    this->GetGameObject()->GetComponent<TransformCom>()->SetLocalPosition({ (float)(memoryId * divideTexSize), localPosition.y ,localPosition.z });
    spc.easingposition = { worldPosition.x,worldPosition.y };
    if (!isDebug)
    {
        //現在のHPからメモリの場所を特定
        int memoryCount = valueRate * 10;
        if (memoryId == memoryCount && !onceFLG) {
            easingFLG = true;
            onceFLG = true;
        }

        if (easingFLG) {
            spc.color = { spc.color.x,spc.color.y,spc.color.z,1 };
            EasingPlay();
            easingFLG = false;
        }
        if (!IsPlayEasing()) {
            spc.color = { spc.color.x,spc.color.y,spc.color.z,0 };
        }

        if (character.lock()->GetComponent<CharaStatusCom>()->IsDeathFrame()) {
            onceFLG = false;
        }
    }
    else {
        spc.color = { spc.color.x,spc.color.y,spc.color.z,1 };
    }
    this->UiSystem::Update(elapsedTime);
}

void UI_HPEffect::OnGUI()
{
    ImGui::InputInt("divideTexSize", &divideTexSize);
    ImGui::Checkbox("Debug", &isDebug);
    this->UiSystem::OnGUI();
}

UI_BoosGauge::UI_BoosGauge()
{
    //ゲージのマスク
    mask = GameObjectManager::Instance().Create();
    mask->SetName("Mask");
    mask->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/BoostMaskVer2.ui", Sprite::SpriteShader::DEFALT, false);

    //ゲージの外枠
    frame = GameObjectManager::Instance().Create();
    frame->SetName("GaugeFrame");
    frame->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/BoostFrameVer2.ui", Sprite::SpriteShader::DEFALT, false);

    //ゲージ本体
    gauge = GameObjectManager::Instance().Create();
    gauge->SetName("Gauge");
    gauge->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/BoostGaugeVer2.ui", Sprite::SpriteShader::DEFALT, false);
}

void UI_BoosGauge::Start()
{
    this->GetGameObject()->AddChildObject(frame);
    this->GetGameObject()->AddChildObject(mask);
    this->GetGameObject()->AddChildObject(gauge);
}

void UI_BoosGauge::Update(float elapsedTime)
{
    std::weak_ptr<GameObject>player = GameObjectManager::Instance().Find("player");
    float dushGauge = *player.lock()->GetComponent<CharacterCom>()->GetDashGauge();
    float maxDushgauge = player.lock()->GetComponent<CharacterCom>()->GetDahsGaugeMax();

    //ゲージの倍率を求める
    float  valueRate = dushGauge / maxDushgauge;
    valueRate = 1 - valueRate;
    float addAngle = maxAngle * valueRate;

    mask->GetComponent<UiSystem>()->spc.angle = minAngle + addAngle;
    mask->GetComponent<UiSystem>()->spc.angle = Mathf::Clamp(mask->GetComponent<UiSystem>()->spc.angle, minAngle, maxAngle);
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

UI_UltNum::UI_UltNum()
{
    std::shared_ptr<GameObject> ultFrame = GameObjectManager::Instance().Find("UltNum");
    std::shared_ptr<GameObject> obj = ultFrame->AddChildObject();
    obj->SetName("ultNumFont");
    std::shared_ptr<Font> font = obj->AddComponent<Font>("Data/Texture/Font/BitmapFont.font", 1024);
    font->position = { 950,886 };
    font->str = L"";  //L付けてね
    font->scale = 0.75f;
    font->color = { 1,1,1,0.75 };
}

void UI_UltNum::Update(float elapsedTIme)
{
    std::shared_ptr<GameObject> font = GameObjectManager::Instance().Find("ultNumFont");
    std::shared_ptr<GameObject> player = GameObjectManager::Instance().Find("player");
    float  ultrate = *player->GetComponent<CharacterCom>()->GetUltGauge() / player->GetComponent<CharacterCom>()->GetUltGaugeMax();
    ultrate *= 100;
    if (int(ultrate) >= 10) {
        font->GetComponent<Font>()->position = { 939,886 };
        if (int(ultrate) >= 100) {
            font->GetComponent<Font>()->position = { 930,886 };
        }
    }
    else {
        font->GetComponent<Font>()->position = { 950,886 };
    }

    std::wstring numstr = std::to_wstring(int(ultrate));
    font->GetComponent<Font>()->str = numstr;
}

UI_SkillNum::UI_SkillNum(CharacterCom::SkillCoolID skillid, const char* objnamekunn, const char* name, DirectX::XMFLOAT2 pos)
{
    std::shared_ptr<GameObject> skillFrame = GameObjectManager::Instance().Find(name);
    std::shared_ptr<GameObject> obj = skillFrame->AddChildObject();
    obj->SetName(objnamekunn);
    std::shared_ptr<Font> font = obj->AddComponent<Font>("Data/Texture/Font/BitmapFont.font", 1024);
    font->position = { pos };
    font->str = L"";  //L付けてね
    font->scale = 0.75f;
    font->color = { 1,1,1,1.3f };
    skill = skillid;
    objname = objnamekunn;
}

void UI_SkillNum::Update(float elapsedTime)
{
    std::shared_ptr<GameObject> font = GameObjectManager::Instance().Find(objname);
    std::shared_ptr<GameObject> player = GameObjectManager::Instance().Find("player");
    auto& playerchara = player->GetComponent<CharacterCom>();
    float skillCoolTime = playerchara->GetSkillCoolTime(skill);
    float skillCoolTimer = *playerchara->GetSkillCoolTimerPointer(skill);
    font->SetEnabled(!playerchara->IsSkillCoolMax(skill));

    // 残り時間を整数値で計算
    float remainingTime = (skillCoolTime - skillCoolTimer) * skillCoolTime; // 残り時間
    int remainingTimeInt = static_cast<int>(remainingTime); // 整数値に変換

    // 数値を文字列に変換
    std::wstring numstr = std::to_wstring(remainingTimeInt);

    // フォントコンポーネントに設定
    font->GetComponent<Font>()->str = numstr;
}

UI_HpNum::UI_HpNum()
{
    std::shared_ptr<GameObject> hpFrame = GameObjectManager::Instance().Find("HpNum");
    std::shared_ptr<GameObject> obj = hpFrame->AddChildObject();
    obj->SetName("hpNumFont");
    std::shared_ptr<Font> font = obj->AddComponent<Font>("Data/Texture/Font/BitmapFont.font", 1024);
    font->str = L"";  //L付けてね
    font->scale = 0.75f;
    font->color = { 1,1,1,1.0f };
    font->position = { 176.0f,860.0f };
}

void UI_HpNum::Update(float elapsedTime)
{
    std::shared_ptr<GameObject> font = GameObjectManager::Instance().Find("hpNumFont");
    std::shared_ptr<GameObject> player = GameObjectManager::Instance().Find("player");

    auto& playerchara = player->GetComponent<CharaStatusCom>();

    float currenthp = *playerchara->GetHitPoint();  // 現在のHP

    // HPが0未満にならないように制限
    if (currenthp < 0)
    {
        currenthp = 0;
    }

    // 残りHPを整数値で計算
    int remainingHpInt = static_cast<int>(currenthp); // 現在のHPを整数に変換

    // 数値を文字列に変換
    std::wstring numstr = std::to_wstring(remainingHpInt);

    // フォントコンポーネントに設定
    font->GetComponent<Font>()->str = numstr;
}

UI_BulletNum::UI_BulletNum()
{
    std::shared_ptr<GameObject> bulletframe = GameObjectManager::Instance().Find("BulletNum");
    std::shared_ptr<GameObject> obj = bulletframe->AddChildObject();
    obj->SetName("bulletNumFont");
    std::shared_ptr<Font> font = obj->AddComponent<Font>("Data/Texture/Font/BitmapFont.font", 1024);
    font->str = L"";  //L付けてね
    font->scale = 0.8f;
    font->color = { 1,1,1,1.0f };
    font->position = { 1644.0f,902.0f };
}

void UI_BulletNum::Update(float elapsedTime)
{
    std::shared_ptr<GameObject> font = GameObjectManager::Instance().Find("bulletNumFont");
    std::shared_ptr<GameObject> player = GameObjectManager::Instance().Find("player");

    auto& playerchara = player->GetComponent<CharacterCom>();

    int currentbullet = *playerchara->GetCurrentBulletNumPointer();  // 現在の弾数

    //弾数が0未満にならないように制限
    if (currentbullet < 0)
    {
        currentbullet = 0;
    }

    // 残りHPを整数値で計算
    int remainingHpInt = currentbullet; // 現在の弾数を整数に変換

    // 数値を文字列に変換
    std::wstring numstr = std::to_wstring(remainingHpInt);

    // フォントコンポーネントに設定
    font->GetComponent<Font>()->str = numstr;
}

void UI_EnemyHp::Update(float elasedTime)
{
    if (enemyFLG) {
        GaugeUpdate(elasedTime);
    }
}

void UI_EnemyHp::GaugeUpdate(float elapsedTime)
{
    float* Hp = this->GetGameObject()->GetComponent<CharaStatusCom>()->GetHitPoint();
    float MaxHp = this->GetGameObject()->GetComponent<CharaStatusCom>()->GetMaxHitpoint();
    std::shared_ptr<UiGauge> gauge = enemyHp->GetComponent<UiGauge>();
    //
   //前フレームのHpと現在のHpが違うなら
    if (*Hp != oldHp) {
        displayFLG = true;
        timer = 0.0f;
        oldHp = *Hp;
        gauge->GetGameObject()->SetEnabled(true);
    }

    if (displayFLG) {
        timer += elapsedTime;
        if (time < timer) {
            displayFLG = false;
            timer = 0.0f;
        }
        gauge->spc.color = { 1,0,0,1 };
    }
    else {
        gauge->GetGameObject()->SetEnabled(false);
    }
}

void UI_EnemyHp::Register(std::weak_ptr<GameObject> obj)
{
    hp = obj.lock()->GetComponent<CharaStatusCom>()->GetHitPoint();
    enemyHp = this->GetGameObject()->AddChildObject();
    enemyHp->SetName("enemyHp");
    enemyHp->AddComponent<UiGauge>("Data/SerializeData/UIData/Player/EnemyHp.ui", Sprite::SpriteShader::DEFALT, false, UiSystem::ChangeValue::X_ONLY_ADD);
    oldHp = *this->GetGameObject()->GetComponent<CharaStatusCom>()->GetHitPoint();
    enemyFLG = true;
    std::shared_ptr<UiGauge> gauge = enemyHp->GetComponent<UiGauge>();
    float* Hp = this->GetGameObject()->GetComponent<CharaStatusCom>()->GetHitPoint();
    float MaxHp = this->GetGameObject()->GetComponent<CharaStatusCom>()->GetMaxHitpoint();
    gauge->spc.objectname = GetGameObject()->GetName();
    gauge->originalTexSize = { 200,100 };
    gauge->SetVariableValue(Hp);
    gauge->SetMaxValue(MaxHp);
}

UI_GameJudge::UI_GameJudge(PVPGameSystem::TEAM_KIND victryTeam)
{
    std::shared_ptr<GameObject> fade = GameObjectManager::Instance().Create();
    fade->SetName("Fade");
    fade->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/GameJudge/Fade.ui", Sprite::SpriteShader::DEFALT, false);

    std::shared_ptr<GameObject> leftTrunder = GameObjectManager::Instance().Create();
    leftTrunder->SetName("LeftTrunder");
    leftTrunder->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/GameJudge/LeftTunder.ui", Sprite::SpriteShader::DEFALT, false);
    leftTrunder->SetEnabled(false);

    std::shared_ptr<GameObject> rightTrunder = GameObjectManager::Instance().Create();
    rightTrunder->SetName("RightTrunder");
    rightTrunder->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/GameJudge/RightTunder.ui", Sprite::SpriteShader::DEFALT, false);
    rightTrunder->SetEnabled(false);

    std::shared_ptr<GameObject> font = GameObjectManager::Instance().Create();
    font->SetName("Font");
    font->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/GameJudge/Font.ui", Sprite::SpriteShader::DEFALT, false);
    font->SetEnabled(false);
    font->GetComponent<UiSystem>()->spc.scale = { 0,0 };

    for (int i = 0; i <= 3; i++)
    {
        std::shared_ptr<GameObject> circle = GameObjectManager::Instance().Create();
        std::string name = "circle_" + std::to_string(i);
        circle->SetName(name.c_str());
        circle->AddComponent<UiSystem>(nullptr, Sprite::SpriteShader::DEFALT, false);
        circles.emplace_back(circle);
    }
    std::weak_ptr<GameObject> player = GameObjectManager::Instance().Find("player");

    //勝敗に応じてロードするテクスチャ変更
    if (victryTeam == player.lock()->GetComponent<CharacterCom>()->GetNetCharaData().GetTeamID()) {
        font->GetComponent<UiSystem>()->LoadTexture("Data/Texture/PlayerUI/Victory.png");

        leftTrunder->GetComponent<UiSystem>()->SetColumns(5);
        leftTrunder->GetComponent<UiSystem>()->SetRows(2);
        leftTrunder->GetComponent<UiSystem>()->SetFrameRate(10.0f);
        leftTrunder->GetComponent<UiSystem>()->spc.color = { 1.000f, 0.933f, 0.000f, 1.000f };

        rightTrunder->GetComponent<UiSystem>()->SetColumns(5);
        rightTrunder->GetComponent<UiSystem>()->SetRows(2);
        rightTrunder->GetComponent<UiSystem>()->SetFrameRate(10.0f);
        rightTrunder->GetComponent<UiSystem>()->spc.color = { 1.000f, 0.933f, 0.000f, 1.000f };
    }
    else {
        font->GetComponent<UiSystem>()->LoadTexture("Data/Texture/PlayerUI/Defeat.png");

        leftTrunder->GetComponent<UiSystem>()->SetColumns(5);
        leftTrunder->GetComponent<UiSystem>()->SetRows(2);
        leftTrunder->GetComponent<UiSystem>()->SetFrameRate(10.0f);
        leftTrunder->GetComponent<UiSystem>()->spc.color = { 1,0,0,1 };

        rightTrunder->GetComponent<UiSystem>()->SetColumns(5);
        rightTrunder->GetComponent<UiSystem>()->SetRows(2);
        rightTrunder->GetComponent<UiSystem>()->SetFrameRate(10.0f);
        rightTrunder->GetComponent<UiSystem>()->spc.color = { 1,0,0,1 };
    }
}

void UI_GameJudge::Start()
{
    this->GetGameObject()->AddChildObject(GameObjectManager::Instance().Find("Fade"));
    this->GetGameObject()->AddChildObject(GameObjectManager::Instance().Find("LeftTrunder"));
    this->GetGameObject()->AddChildObject(GameObjectManager::Instance().Find("RightTrunder"));
    this->GetGameObject()->AddChildObject(GameObjectManager::Instance().Find("Font"));
    for (auto& circle : circles)
    {
        this->GetGameObject()->AddChildObject(circle.lock());
    }
}
void UI_GameJudge::OnGUI() {
    ImGui::DragInt("state", &state);
}

void UI_GameJudge::Update(float elaspedTime)
{
    std::shared_ptr<GameObject> fade = GameObjectManager::Instance().Find("Fade");
    std::shared_ptr<UiSystem> fadeCom = fade->GetComponent<UiSystem>();

    std::shared_ptr<GameObject> leftTrunder = GameObjectManager::Instance().Find("LeftTrunder");
    std::shared_ptr<UiSystem> leftTrunderCom = leftTrunder->GetComponent<UiSystem>();

    std::shared_ptr<GameObject> rightTrunder = GameObjectManager::Instance().Find("RightTrunder");
    std::shared_ptr<UiSystem> rightTrunderCom = rightTrunder->GetComponent<UiSystem>();

    std::shared_ptr<GameObject> font = GameObjectManager::Instance().Find("Font");
    std::shared_ptr<UiSystem> fontCom = font->GetComponent<UiSystem>();

    switch (state)
    {
    case 0:
        fadeTimr += 0.1f;
        fadeCom->spc.color = { 0,0,0,fadeTimr };
        if (fadeTimr >= 1.0f) {
            state++;
        }
        break;
    case 1:
        font->SetEnabled(true);
        fontCom->spc.scale = { fontCom->spc.scale.x + 0.01f, fontCom->spc.scale.y + 0.01f, };
        if (fontCom->spc.scale.x >= 1.0f) {
            state++;
        }
        break;

    case 2:
        leftTrunder->SetEnabled(true);
        rightTrunder->SetEnabled(true);
        break;
    default:
        break;
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
    //ブースト
    CreateBoostUI();

    //全員の使用キャラ表示
    CreateNetUseCharaUI();

    //キャラアイコン
    CreatePlayerIcon();

    //銃のアイコンとか
    CreateGunIcon();

    //Hitエフェクト
    CreateHitEffect();

    //キル時のエフェクト
    CreateKillEffect();

    //キルログ
    CreateKillLog();

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
    //キルログ
    KillLogUpdate(elapsedTime);

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

    //位置をずらす定数
    const DirectX::XMFLOAT2 offset = { -95.0f,-6.0f };
    CharacterCom::SkillCoolID skillNum;
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
        name += "skill_RightClick.png";
        iconName += "mouse_right_outline.png";
        skillNum = CharacterCom::SkillCoolID::RightClick;
        break;
    default:
        break;
    }

    std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");

    //SkillFrame
    std::shared_ptr<GameObject> skillFrame = canvas->AddChildObject();
    skillFrame->SetName("Skill_Frame");
    auto& a = skillFrame->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/SkillFrame1_02.ui", Sprite::SpriteShader::DEFALT, false);
    a->spc.position = { a->spc.position.x - (count * offset.x),a->spc.position.y - (count * offset.y) };

    switch (use_skill)
    {
    case E:

        //SkillMask
    {
        std::shared_ptr<GameObject> skillGaueHide = skillFrame->AddChildObject();
        skillGaueHide->SetName("E_SkillGaugeHide");
        auto& a = skillGaueHide->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/SkillGaugeMask.ui", Sprite::SpriteShader::DEFALT, false);
        a->spc.position = { a->spc.position.x - (count * offset.x),a->spc.position.y - (count * offset.y) };
    }

    //SkillGauge
    {
        std::shared_ptr<GameObject> skillGauge = skillFrame->AddChildObject();
        skillGauge->SetName("E_SkillGauge");
        std::shared_ptr<UI_Skill>skillGaugeCmp = skillGauge->AddComponent<UI_Skill>("Data/SerializeData/UIData/Player/SkillGauge1.ui", Sprite::SpriteShader::DEFALT, false, 997, 908 - count * offset.y);
        std::shared_ptr<GameObject>player = GameObjectManager::Instance().Find("player");
        skillGaugeCmp->SetMaxValue(player->GetComponent<CharacterCom>()->GetSkillCoolTime(skillNum));
        float* i = player->GetComponent<CharacterCom>()->GetSkillCoolTimerPointer(skillNum);
        skillGaugeCmp->SetVariableValue(i);
        skillGaugeCmp->spc.position = { skillGaugeCmp->spc.position.x - (count * offset.x),skillGaugeCmp->spc.position.y };
    }

    //Skill_E
    {
        std::shared_ptr<GameObject> skillIcon = skillFrame->AddChildObject();
        skillIcon->SetName("Skill_E");
        auto& a = skillIcon->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/Skill_E.ui", Sprite::SpriteShader::DEFALT, false);
        skillIcon->GetComponent<UiSystem>()->LoadTexture(name);
        a->spc.position = { a->spc.position.x - (count * offset.x),a->spc.position.y - (count * offset.y) };
    }

    //skillNum
    {
        std::shared_ptr<GameObject> skillnum = skillFrame->AddChildObject();
        skillnum->SetName("E_SkillNum");
        DirectX::XMFLOAT2 pos = { 1343.0f,883.0f };
        std::shared_ptr<UI_SkillNum>skillnumCom = skillnum->AddComponent<UI_SkillNum>(skillNum, "E_SkillNumFont", "E_SkillNum", pos);
    }

    //KeyBoardIcon
    {
        std::shared_ptr<GameObject> skillIcon = skillFrame->AddChildObject();
        skillIcon->SetName("E_KeyIcon");
        auto& a = skillIcon->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/KeyIcon.ui", Sprite::SpriteShader::DEFALT, false);
        skillIcon->GetComponent<UiSystem>()->LoadTexture(iconName);
        a->spc.position = { a->spc.position.x - (count * offset.x),a->spc.position.y - (count * offset.y) };
    }

    //Thunder
    {
        std::shared_ptr<GameObject> thunder = skillFrame->AddChildObject();
        thunder->SetName("E_Thunder");
        thunder->AddComponent<UI_SkillComp>(skillNum);
        auto& a = thunder->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/skill_thunder.ui", Sprite::SpriteShader::DEFALT, false);
        a->SetColumns(10);
        a->SetRows(1);
        a->SetFrameRate(30.0f);
        a->spc.position = { a->spc.position.x - (count * offset.x),a->spc.position.y - (count * offset.y) };
    }

    break;

    case RIGHT_CLICK:

        //SkillMask
    {
        std::shared_ptr<GameObject> skillGaueHide = skillFrame->AddChildObject();
        skillGaueHide->SetName("RightClick_SkillGaugeHide");
        auto& a = skillGaueHide->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/SkillGaugeMask.ui", Sprite::SpriteShader::DEFALT, false);
        a->spc.position = { a->spc.position.x - (count * offset.x),a->spc.position.y - (count * offset.y) };
    }

    //SkillGauge
    {
        std::shared_ptr<GameObject> skillGauge = skillFrame->AddChildObject();
        skillGauge->SetName("RightClick_SkillGauge");
        std::shared_ptr<UI_Skill>skillGaugeCmp = skillGauge->AddComponent<UI_Skill>("Data/SerializeData/UIData/Player/SkillGauge1.ui", Sprite::SpriteShader::DEFALT, false, 997, 908 - count * offset.y);
        std::shared_ptr<GameObject>player = GameObjectManager::Instance().Find("player");
        skillGaugeCmp->SetMaxValue(player->GetComponent<CharacterCom>()->GetSkillCoolTime(skillNum));
        float* i = player->GetComponent<CharacterCom>()->GetSkillCoolTimerPointer(skillNum);
        skillGaugeCmp->SetVariableValue(i);
        skillGaugeCmp->spc.position = { skillGaugeCmp->spc.position.x - (count * offset.x),skillGaugeCmp->spc.position.y };
    }

    //Skill_RightClick
    {
        std::shared_ptr<GameObject> skillIcon = skillFrame->AddChildObject();
        skillIcon->SetName("Skill_RightClick");
        auto& a = skillIcon->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/Skill_E.ui", Sprite::SpriteShader::DEFALT, false);
        skillIcon->GetComponent<UiSystem>()->LoadTexture(name);
        a->spc.position = { a->spc.position.x - (count * offset.x),a->spc.position.y - (count * offset.y) };
    }

    //skillNum
    {
        std::shared_ptr<GameObject> skillnum = skillFrame->AddChildObject();
        skillnum->SetName("RIGHT_SkillNum");
        DirectX::XMFLOAT2 pos = { 1435.0f,890.0f };
        std::shared_ptr<UI_SkillNum>skillnumCom = skillnum->AddComponent<UI_SkillNum>(skillNum, "RIGHT_SkillNumFont", "RIGHT_SkillNum", pos);
    }

    //KeyBoardIcon
    {
        std::shared_ptr<GameObject> skillIcon = skillFrame->AddChildObject();
        skillIcon->SetName("RightClick_KeyIcon");
        auto& a = skillIcon->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/KeyIcon.ui", Sprite::SpriteShader::DEFALT, false);
        skillIcon->GetComponent<UiSystem>()->LoadTexture(iconName);
        a->spc.position = { a->spc.position.x - (count * offset.x),a->spc.position.y - (count * offset.y) };
    }

    //Thunder
    {
        std::shared_ptr<GameObject> thunder = skillFrame->AddChildObject();
        thunder->SetName("RightClick_Thunder");
        thunder->AddComponent<UI_SkillComp>(skillNum);
        auto& a = thunder->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/skill_thunder.ui", Sprite::SpriteShader::DEFALT, false);
        a->SetColumns(10);
        a->SetRows(1);
        a->SetFrameRate(30.0f);
        a->spc.position = { a->spc.position.x - (count * offset.x),a->spc.position.y - (count * offset.y) };
    }

    break;
    }
}

void PlayerUIManager::CreateReticleUI()
{
    //ロードするテクスチャを設定
    std::string name = "Data/Texture/PlayerUI/" + (std::string)player.lock()->GetComponent<CharacterCom>()->GetName() + "/Sight.png";
    std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");
    std::shared_ptr<GameObject> reticle = canvas->AddChildObject();
    reticle->SetName("reticle");
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

    //ビリビリ右
    {
        std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("UltFrame");
        std::shared_ptr<GameObject> ultthunder = canvas->AddChildObject();
        ultthunder->SetName("UltThunder_Right");
        std::shared_ptr<Sprite> s = ultthunder->AddComponent<Sprite>("Data/SerializeData/UIData/Player/ult_thunder_right.ui", Sprite::SpriteShader::DEFALT, false);
        s->SetColumns(5);
        s->SetRows(5);
        s->SetFrameRate(20.0f);
        ultthunder->SetEnabled(false);
    }

    //ビリビリ左
    {
        std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("UltFrame");
        std::shared_ptr<GameObject> ultthunder = canvas->AddChildObject();
        ultthunder->SetName("UltThunder_Left");
        std::shared_ptr<Sprite> s = ultthunder->AddComponent<Sprite>("Data/SerializeData/UIData/Player/ult_thunder_left.ui", Sprite::SpriteShader::DEFALT, false);
        s->SetColumns(5);
        s->SetRows(5);
        s->SetFrameRate(20.0f);
        ultthunder->SetEnabled(false);
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

        std::shared_ptr<UI_Skill>ultGaugeCmp = ultGauge->AddComponent<UI_Skill>("Data/SerializeData/UIData/Player/UltGauge.ui", Sprite::SpriteShader::DEFALT, false, 1190, 960);
        std::shared_ptr<GameObject>player = GameObjectManager::Instance().Find("player");
        ultGaugeCmp->SetMaxValue(player->GetComponent<CharacterCom>()->GetUltGaugeMax());
        float* i = player->GetComponent<CharacterCom>()->GetUltGauge();
        ultGaugeCmp->SetVariableValue(i);
    }
    //ultNum
    {
        std::shared_ptr<GameObject> ultFrame = GameObjectManager::Instance().Find("UltFrame");
        std::shared_ptr<GameObject> ultNum = ultFrame->AddChildObject();
        ultNum->SetName("UltNum");
        std::shared_ptr<UI_UltNum>ultnumCom = ultNum->AddComponent<UI_UltNum>();
    }

    //Rkey
    {
        std::shared_ptr<GameObject> ultFrame = GameObjectManager::Instance().Find("UltFrame");
        std::shared_ptr<GameObject> Rkey = ultFrame->AddChildObject();
        Rkey->SetName("Rkey");
        Rkey->AddComponent<Sprite>("Data/SerializeData/UIData/Player/R_keyIcon.ui", Sprite::SpriteShader::DEFALT, false);
    }

    //ビリビリ雷
    {
        std::shared_ptr<GameObject> ultFrame = GameObjectManager::Instance().Find("UltFrame");
        std::shared_ptr<GameObject> ultthunder = ultFrame->AddChildObject();
        ultthunder->SetName("UltThunder_IN");
        std::shared_ptr<Sprite> s = ultthunder->AddComponent<Sprite>("Data/SerializeData/UIData/Player/ult_thunder_in.ui", Sprite::SpriteShader::DEFALT, false);
        s->SetColumns(5);
        s->SetRows(5);
        s->SetFrameRate(20.0f);
        ultthunder->SetEnabled(false);
    }
}
void PlayerUIManager::CreateHpUI()
{
    std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");

    std::shared_ptr<GameObject> Hp = GameObjectManager::Instance().Create();
    Hp->SetName("HpUI");
    Hp->AddComponent<UI_PlayerHpUI>();
    //HPエフェクト
    {
        for (int i = 0; i < 9; i++) {
            std::shared_ptr<GameObject> hpgauge = GameObjectManager::Instance().Find("HpGauge");
            std::shared_ptr<GameObject> hpEffect = hpgauge->AddChildObject();
            std::string name = "HpEffect_" + std::to_string(i);
            hpEffect->SetName(name.c_str());
            int gaugeTexSize = hpgauge->GetComponent<UiGauge>()->originalTexSize.x;
            std::shared_ptr<UI_HPEffect>gauge = hpEffect->AddComponent<UI_HPEffect>("Data/SerializeData/UIData/Player/HpEffect.ui", Sprite::SpriteShader::DEFALT, true, gaugeTexSize, player, i);
        }
    }

    //自身の名前
    {
        std::shared_ptr<GameObject> hpFrame = GameObjectManager::Instance().Find("HpFrame");
        std::shared_ptr<GameObject> playerName = hpFrame->AddChildObject();
        playerName->SetName("PlayerName");
        std::shared_ptr<Font> font = playerName->AddComponent<Font>("Data/Texture/Font/BitmapFont.font", 1024);
        font->scale = 0.6f;
        font->color = { 1,1,1,1.0f };
        font->position = { 177.0f,987.0f };
        font->str = UTF8ToWString3(GameObjectManager::Instance().Find("player")->GetComponent<CharacterCom>()->GetNetCharaData().GeNetName());  //L付けてね
    }
}
void PlayerUIManager::CreateBoostUI()
{
    //Boost
    {
        std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");
        std::shared_ptr<GameObject> hpMemori = canvas->AddChildObject();
        hpMemori->SetName("boostGauge");

        hpMemori->AddComponent<UI_BoosGauge>();
    }
}

void PlayerUIManager::CreatePlayerIcon()
{
    //ロードするテクスチャを設定
    std::string name = "Data/Texture/PlayerUI/CharaIcon/" + (std::string)player.lock()->GetComponent<CharacterCom>()->GetName() + ".png";
    std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");

    std::shared_ptr<GameObject>char_fire = canvas->AddChildObject();
    char_fire->SetName("char_fire");
    auto& fire = char_fire->AddComponent<Sprite>("Data/SerializeData/UIData/Player/chara_fire.ui", Sprite::SpriteShader::DEFALT, false);
    fire->SetColumns(5);
    fire->SetRows(4);
    fire->SetFrameRate(20.0f);
    fire->SetEnabled(false);

    std::shared_ptr<GameObject> charaicon = char_fire->AddChildObject();
    charaicon->SetName("CharaIcon");
    auto& a = charaicon->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/CharaIcon.ui", Sprite::SpriteShader::DEFALT, false);
    a->LoadTexture(name);

    std::shared_ptr<GameObject> dokuroicon = char_fire->AddChildObject();
    dokuroicon->SetName("Dokuro");
    dokuroicon->AddComponent<UI_DeathComp>();
    auto& dokurosprite = dokuroicon->AddComponent<Sprite>("Data/SerializeData/UIData/Player/dokuro.ui", Sprite::SpriteShader::DEFALT, false);
}

//銃のアイコンとか
void PlayerUIManager::CreateGunIcon()
{
    //ロードするテクスチャを設定
    std::string name = "Data/Texture/PlayerUI/weapon_icon.png";

    {
        std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");
        std::shared_ptr<GameObject> gunicon = canvas->AddChildObject();
        gunicon->SetName("GunIcon");
        auto& spritegun = gunicon->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/MainWeponIcon.ui", Sprite::SpriteShader::DEFALT, false);
        spritegun->LoadTexture(name);
    }

    //上側の線
    {
        std::shared_ptr<GameObject> gunicon = GameObjectManager::Instance().Find("GunIcon");
        std::shared_ptr<GameObject> up = gunicon->AddChildObject();
        up->SetName("upbar");
        auto& a = up->AddComponent<Sprite>("Data/SerializeData/UIData/Player/upbar.ui", Sprite::SpriteShader::DEFALT, false);
    }

    //下側の線
    {
        std::shared_ptr<GameObject> gunicon = GameObjectManager::Instance().Find("GunIcon");
        std::shared_ptr<GameObject> down = gunicon->AddChildObject();
        down->SetName("downbar");
        auto& a = down->AddComponent<Sprite>("Data/SerializeData/UIData/Player/downbar.ui", Sprite::SpriteShader::DEFALT, false);
    }

    //max弾数
    {
        std::shared_ptr<GameObject> gunicon = GameObjectManager::Instance().Find("GunIcon");
        std::shared_ptr<GameObject> bulettonum = gunicon->AddChildObject();
        std::shared_ptr<GameObject> player = GameObjectManager::Instance().Find("player");
        auto& playerchara = player->GetComponent<CharacterCom>();
        bulettonum->SetName("MaxBuletto");
        float maxhp = playerchara->GetMaxBulletNum();  //最大弾数
        std::wstring numstr = std::to_wstring(static_cast<int>(maxhp));
        std::shared_ptr<Font> font = bulettonum->AddComponent<Font>("Data/Texture/Font/BitmapFont.font", 1024);
        font->scale = 0.8f;
        font->color = { 1,1,1,0.7f };
        font->str = numstr;  //L付けてね
        font->position = { 1699.0f,905.0f };
    }

    //current弾数
    {
        std::shared_ptr<GameObject> gunicon = GameObjectManager::Instance().Find("GunIcon");
        std::shared_ptr<GameObject> bulletnum = gunicon->AddChildObject();
        bulletnum->SetName("BulletNum");
        std::shared_ptr<UI_BulletNum>bn = bulletnum->AddComponent<UI_BulletNum>();
    }

    //真ん中の線
    {
        std::shared_ptr<GameObject> gunicon = GameObjectManager::Instance().Find("GunIcon");
        std::shared_ptr<GameObject> middle = gunicon->AddChildObject();
        middle->SetName("middlebar");
        auto& a = middle->AddComponent<Sprite>("Data/SerializeData/UIData/Player/middlebar.ui", Sprite::SpriteShader::DEFALT, false);
    }
}

//ヒットエフェクト
void PlayerUIManager::CreateHitEffect()
{
    bool* isHit = player.lock()->GetComponent<CharacterCom>()->GetIsHitAttack();

    //Boost
    {
        std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");
        std::shared_ptr<GameObject> hit = canvas->AddChildObject();
        hit->SetName("HitEffect");

        hit->AddComponent<UiFlag>("Data/SerializeData/UIData/Player/HitEffect.ui", Sprite::SpriteShader::DEFALT, false, isHit);
    }
}

void PlayerUIManager::CreateKillEffect()
{
    //Boost
    {
        std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");
        std::shared_ptr<GameObject> hit = canvas->AddChildObject();
        hit->SetName("KillEffect");

        hit->AddComponent<UI_KillEffect>();
    }
}

//キルログ
void PlayerUIManager::KillLogUpdate(float elapsedTime)
{
    //タイマー更新
    for (int deathPID = 0; deathPID < 4; deathPID++)
    {
        kilogTimer[deathPID] -= elapsedTime;
    }

    //キルした側
    for (int killPID = 0; killPID < 4; killPID++)
    {
        //デスした側
        for (int deathPID = 0; deathPID < 4; deathPID++)
        {
            auto& killflg = StaticSendDataManager::Instance().GetKillID(killPID, deathPID);
            if (killflg)  //キルが発生しているなら
            {
                //一回だけ通るように
                if (kilogTimer[deathPID] < 0)
                {
                    kilogTimer[deathPID] = 3;

                    //ここでキルログを出す
                    int killChara = -1;
                    DeathData  d;
                    //チームを見る
                    for (auto& chara : GameObjectManager::Instance().GetCharaObject())
                    {
                        if (!chara.lock())continue;
                        auto& charaCom = chara.lock()->GetComponent<CharacterCom>();

                        if (!charaCom)continue;

                        //キル側
                        if (charaCom->GetNetCharaData().GetNetPlayerID() == killPID)
                        {
                            killChara = charaCom->GetNetCharaData().GetCharaID();

                            //自分の場合
                            if (std::strcmp(chara.lock()->GetName(), "player") == 0)
                                d.myID = 0;
                        }

                        //デス側
                        if (charaCom->GetNetCharaData().GetNetPlayerID() == deathPID)
                        {
                            //自分の場合
                            if (std::strcmp(chara.lock()->GetName(), "player") == 0)
                                d.myID = 1;
                            //チームを比べる
                            auto& player = GameObjectManager::Instance().Find("player");
                            auto& charaC = player->GetComponent<CharacterCom>();
                            int pT = charaC->GetNetCharaData().GetTeamID();
                            int nT = charaCom->GetNetCharaData().GetTeamID();
                            d.isEnemy = (pT != nT);

                            d.charaID = charaCom->GetNetCharaData().GetCharaID();

                            //使用UIを決める
                            int uiID = 0;
                            for (auto& log : saveCharaKilog)
                            {
                                //後から追加される数を増やす
                                log.second.moveData.underNum++;
                                if (log.second.isEnemy != d.isEnemy)continue;   //同じチームが流れている場合は入る

                                if (log.second.moveData.id == 0)
                                    uiID = 1;
                            }
                            if (uiID == 1)d.moveData.id = 0;
                            else d.moveData.id = 1;
                        }
                    }

                    //キルが起きたので一旦保存
                    saveCharaKilog[killChara] = d;
                }
            }
            killflg = false;
        }
    }

    std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("killLogCanvas");

    //削除用変数
    std::vector<int> removeID;

    //演出用変数
    static const float stopY = 300; //停止位置
    static const float stopX = 3000; //停止位置
    static const float stopA = 0.6f; //停止透明色
    static const float removeTime = 5; //消去時間

    //敵味方、関係なく表示する
    auto& kilogView = [&](std::string parentObjName, std::pair<const int, DeathData>& data)
        {
            auto& moveData = data.second.moveData;

            auto& parant = canvas->GetChildFind((parentObjName + std::to_string(moveData.id)).c_str());
            auto& c01 = parant->GetChildFind("charaView01");
            auto& c02 = parant->GetChildFind("charaView02");
            auto& bow = parant->GetChildFind("Bow");

            auto& Pspr = parant->GetComponent<UiSystem>();
            auto& c1spr = c01->GetComponent<UiSystem>();
            auto& c2spr = c02->GetComponent<UiSystem>();

            //起動時
            if (!moveData.startFlg)
            {
                moveData.startFlg = true;
                parant->SetEnabled(true);
                //初期位置
                parant->transform_->SetWorldPosition({ 1760,500,0 });
                c01->transform_->SetLocalPosition({ 80,0,0 });
                c02->transform_->SetLocalPosition({ 380,0,0 });
                bow->transform_->SetLocalPosition({ 0,0,0 });

                //いーじんぐ初期か
                Pspr->spc.color.w = 0;
                c1spr->spc.color.w = 0;
                c2spr->spc.color.w = 0;
                if (data.second.myID == 0) //キルが自分
                    c1spr->spc.color = { 1,0,0,0 };
                if (data.second.myID == 1) //デスが自分
                    c2spr->spc.color = { 1,0,0,0 };

                //キャラIDを見て画像ずらす
                c01->GetComponent<UiSystem>()->numUVScroll.x = 0.25f * data.first;
                c02->GetComponent<UiSystem>()->numUVScroll.x = 0.25f * data.second.charaID;
            }

            //動き
            moveData.timer += elapsedTime;

            //入場演出
            static const float inSlideTime = 0.5f;
            if (moveData.timer <= inSlideTime)
            {
                //位置
                DirectX::XMFLOAT3 pos = parant->transform_->GetWorldPosition();
                pos.y = Mathf::Lerp(pos.y, stopY, moveData.timer / inSlideTime);
                parant->transform_->SetWorldPosition(pos);

                //色
                Pspr->spc.color.w = Mathf::Lerp(Pspr->spc.color.w, stopA, moveData.timer / inSlideTime);
                c1spr->spc.color.w = Mathf::Lerp(c1spr->spc.color.w, 1, moveData.timer / inSlideTime);
                c2spr->spc.color.w = Mathf::Lerp(c2spr->spc.color.w, 1, moveData.timer / inSlideTime);
            }

            //追加された時に上にスライド
            {
                DirectX::XMFLOAT3 pos = parant->transform_->GetWorldPosition();
                pos.y = Mathf::Lerp(pos.y, stopY - moveData.underNum * 100, 0.1f);
                parant->transform_->SetWorldPosition(pos);
            }

            //退出演出
            static const float outSlideTime = 0.5f;
            if (moveData.timer >= removeTime - outSlideTime)
            {
                float t = moveData.timer - (removeTime - outSlideTime);
                DirectX::XMFLOAT3 pos = parant->transform_->GetWorldPosition();
                pos.x = Mathf::Lerp(pos.x, stopX, t / outSlideTime);
                parant->transform_->SetWorldPosition(pos);
            }

            //削除申請
            if (moveData.timer > removeTime)
            {
                parant->SetEnabled(false);
                removeID.emplace_back(data.first);
            }
        };

    for (auto& log : saveCharaKilog)
    {
        if (log.second.isEnemy)  //チームが敵を倒した場合
            kilogView("allyKillLog", log);
        else
            kilogView("enemyKillLog", log);
    }

    for (auto& id : removeID)
    {
        GameObj parent;
        if (saveCharaKilog[id].isEnemy)
            parent = canvas->GetChildFind(("allyKillLog" + std::to_string(saveCharaKilog[id].moveData.id)).c_str());
        else
            parent = canvas->GetChildFind(("enemyKillLog" + std::to_string(saveCharaKilog[id].moveData.id)).c_str());

        parent->SetEnabled(false);

        saveCharaKilog.erase(id);
    }
}

void PlayerUIManager::CreateKillLog()
{
    std::shared_ptr<GameObject> killLogCanvas = GameObjectManager::Instance().Create();
    killLogCanvas->SetName("killLogCanvas");

    //味方用
    for (int i = 0; i < 2; ++i)
    {
        std::shared_ptr<GameObject> allyBack = killLogCanvas->AddChildObject();
        allyBack->SetName(("allyKillLog" + std::to_string(i)).c_str());
        allyBack->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/CharaView/charaListBack.ui", Sprite::SpriteShader::DEFALT, false);
        allyBack->SetEnabled(false);
        //一人目
        {
            std::shared_ptr<GameObject> ally01 = allyBack->AddChildObject();
            ally01->SetName("charaView01");
            ally01->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/CharaView/charaList.ui", Sprite::SpriteShader::DEFALT, false);
        }
        //二人目
        {
            std::shared_ptr<GameObject> ally02 = allyBack->AddChildObject();
            ally02->SetName("charaView02");
            ally02->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/CharaView/charaList.ui", Sprite::SpriteShader::DEFALT, false);
        }
        //矢印
        {
            std::shared_ptr<GameObject> allyBow = allyBack->AddChildObject();
            allyBow->SetName("Bow");
            allyBow->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/CharaView/logBow.ui", Sprite::SpriteShader::DEFALT, false);
        }
    }

    //敵用
    for (int i = 0; i < 2; ++i)
    {
        std::shared_ptr<GameObject> enemyBack = killLogCanvas->AddChildObject();
        enemyBack->SetName(("enemyKillLog" + std::to_string(i)).c_str());
        enemyBack->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/CharaView/charaListBackEnemy.ui", Sprite::SpriteShader::DEFALT, false);
        enemyBack->SetEnabled(false);
        //一人目
        {
            std::shared_ptr<GameObject> ally01 = enemyBack->AddChildObject();
            ally01->SetName("charaView01");
            ally01->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/CharaView/charaList.ui", Sprite::SpriteShader::DEFALT, false);
        }
        //二人目
        {
            std::shared_ptr<GameObject> ally02 = enemyBack->AddChildObject();
            ally02->SetName("charaView02");
            ally02->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/CharaView/charaList.ui", Sprite::SpriteShader::DEFALT, false);
        }
        //矢印
        {
            std::shared_ptr<GameObject> enemyBow = enemyBack->AddChildObject();
            enemyBow->SetName("Bow");
            enemyBow->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/CharaView/logBow.ui", Sprite::SpriteShader::DEFALT, false);
        }
    }
}

void PlayerUIManager::CreateNetUseCharaUI()
{
    std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");
    //味方
    {
        std::shared_ptr<GameObject> allyBack = canvas->AddChildObject();
        allyBack->SetName("allyBack");
        allyBack->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/CharaView/charaListBack.ui", Sprite::SpriteShader::DEFALT, false);
        //一人目
        {
            std::shared_ptr<GameObject> ally01 = allyBack->AddChildObject();
            ally01->SetName("charaView01");
            ally01->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/CharaView/charaList.ui", Sprite::SpriteShader::DEFALT, false);

            //死亡したらの処理
            {
                std::shared_ptr<GameObject>DeathIcon = ally01->AddChildObject();
                DeathIcon->SetName("DeathIcon");
                DeathIcon->AddComponent<Sprite>("Data/SerializeData/UIData/Player/CharaView/net_death_icon1.ui", Sprite::SpriteShader::DEFALT, false);
            }
        }
        //二人目
        {
            std::shared_ptr<GameObject> ally02 = allyBack->AddChildObject();
            ally02->SetName("charaView02");
            ally02->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/CharaView/charaList.ui", Sprite::SpriteShader::DEFALT, false);

            //死亡したらの処理
            {
                std::shared_ptr<GameObject>DeathIcon = ally02->AddChildObject();
                DeathIcon->SetName("DeathIcon");
                DeathIcon->AddComponent<Sprite>("Data/SerializeData/UIData/Player/CharaView/net_death_icon2.ui", Sprite::SpriteShader::DEFALT, false);
            }
        }
    }

    //敵
    {
        std::shared_ptr<GameObject> enemyBack = canvas->AddChildObject();
        enemyBack->SetName("enemyBack");
        enemyBack->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/CharaView/charaListBackEnemy.ui", Sprite::SpriteShader::DEFALT, false);
        //一人目
        {
            std::shared_ptr<GameObject> enemy01 = enemyBack->AddChildObject();
            enemy01->SetName("charaView01");
            enemy01->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/CharaView/charaList.ui", Sprite::SpriteShader::DEFALT, false);

            //死亡したらの処理
            {
                std::shared_ptr<GameObject>DeathIcon = enemy01->AddChildObject();
                DeathIcon->SetName("DeathIcon");
                DeathIcon->AddComponent<Sprite>("Data/SerializeData/UIData/Player/CharaView/net_death_icon3.ui", Sprite::SpriteShader::DEFALT, false);
            }
        }
        //二人目
        {
            std::shared_ptr<GameObject> enemy02 = enemyBack->AddChildObject();
            enemy02->SetName("charaView02");
            enemy02->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/CharaView/charaList.ui", Sprite::SpriteShader::DEFALT, false);

            //死亡したらの処理
            {
                std::shared_ptr<GameObject>DeathIcon = enemy02->AddChildObject();
                DeathIcon->SetName("DeathIcon");
                DeathIcon->AddComponent<Sprite>("Data/SerializeData/UIData/Player/CharaView/net_death_icon4.ui", Sprite::SpriteShader::DEFALT, false);
            }
        }
    }
}

void PlayerUIManager::NetUseCharaUIUpdate(int chara[4], int photonid[4])
{
    std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");

    auto& charaView = [&](std::shared_ptr<GameObject> parentObj, int of)
        {
            auto& c01 = parentObj->GetChildFind("charaView01");
            auto& c02 = parentObj->GetChildFind("charaView02");
            auto& deathicon1 = c01->GetChildFind("DeathIcon");
            auto& sprite1 = c01->GetChildFind("DeathIcon")->GetComponent<Sprite>();
            auto& deathicon2 = c02->GetChildFind("DeathIcon");
            auto& sprite2 = c02->GetChildFind("DeathIcon")->GetComponent<Sprite>();

            //位置
            c01->transform_->SetLocalPosition({ 122,0,0 });
            c02->transform_->SetLocalPosition({ 341,0,0 });

            //位置を無理やり補正
            deathicon1->transform_->SetLocalPosition({ -1573.559f,0,0 });
            deathicon2->transform_->SetLocalPosition({ -1558.791f,0,0 });

            //キャラIDを見て画像ずらす
            if (chara[0 + of] >= 0)
                c01->GetComponent<UiSystem>()->numUVScroll.x = 0.25f * chara[0 + of];
            if (chara[1 + of] >= 0)
                c02->GetComponent<UiSystem>()->numUVScroll.x = 0.25f * chara[1 + of];

            if (photonid[0 + of] >= 0)
            {
                std::string name = "netPlayer" + std::to_string(photonid[0 + of]);
                GameObj netPlayer = GameObjectManager::Instance().Find(name.c_str());

                if (!netPlayer)
                {
                    netPlayer = GameObjectManager::Instance().Find("player");
                }

                auto& characom = netPlayer->GetComponent<CharacterCom>();
                auto& charastatuscom = netPlayer->GetComponent<CharaStatusCom>();

                sprite1->SetEnabled(charastatuscom->IsDeath());

                //イージング発動
                if (charastatuscom->IsDeathFrame())
                {
                    sprite1->EasingPlay();
                }

                //イージング停止
                if (!sprite1->GetEnabled())
                {
                    sprite1->spc.scale = { 0.3f,0.3f };
                    sprite1->spc.color = { 1,1,1,1 };
                }
            }
            if (photonid[1 + of] >= 0)
            {
                std::string name = "netPlayer" + std::to_string(photonid[1 + of]);
                GameObj netPlayer = GameObjectManager::Instance().Find(name.c_str());

                if (!netPlayer)
                {
                    netPlayer = GameObjectManager::Instance().Find("player");
                }

                auto& characom = netPlayer->GetComponent<CharacterCom>();
                auto& charastatuscom = netPlayer->GetComponent<CharaStatusCom>();

                sprite2->SetEnabled(charastatuscom->IsDeath());

                //イージング発動
                if (charastatuscom->IsDeathFrame())
                {
                    sprite2->EasingPlay();
                }

                //イージング停止
                if (!sprite2->GetEnabled())
                {
                    sprite2->spc.scale = { 0.3f,0.3f };
                    sprite2->spc.color = { 1,1,1,1 };
                }
            }
        };

    auto& ally = canvas->GetChildFind("allyBack");
    auto& enemy = canvas->GetChildFind("enemyBack");
    if (ally)
        charaView(ally, 0);
    if (enemy)
        charaView(enemy, 2);
}

void PlayerUIManager::CreateNetTeamUI(std::weak_ptr<GameObject> netPlayer)
{
    allyHp = true;

    //ロードするテクスチャを設定(アイコンができてから)
    std::string name = "Data/Texture/PlayerUI/CharaIcon/" + (std::string)netPlayer.lock()->GetComponent<CharacterCom>()->GetName() + ".png";

    //HpFrame
    {
        std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");
        std::shared_ptr<GameObject> hpFrame = canvas->AddChildObject();
        hpFrame->SetName("AllyHpFrame");
        hpFrame->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/AllyHpFrame.ui", Sprite::SpriteShader::DEFALT, false);
    }

    //HpGauge
    {
        std::shared_ptr<GameObject> hpFrame = GameObjectManager::Instance().Find("AllyHpFrame");
        std::shared_ptr<GameObject> hpGauge = hpFrame->AddChildObject();
        hpGauge->SetName("AllyHpGauge");
        std::shared_ptr<UiGauge>gauge = hpGauge->AddComponent<UiGauge>("Data/SerializeData/UIData/Player/AllyHpGauge.ui", Sprite::SpriteShader::DEFALT, true, UiSystem::X_ONLY_ADD);
        gauge->SetMaxValue(netPlayer.lock()->GetComponent<CharaStatusCom>()->GetMaxHitpoint());
        float* i = netPlayer.lock()->GetComponent<CharaStatusCom>()->GetHitPoint();
        gauge->SetVariableValue(i);
    }

    //Icon
    {
        std::shared_ptr<GameObject> icon = GameObjectManager::Instance().Find("AllyHpFrame")->AddChildObject();
        icon->SetName("AllyIcon");
        std::shared_ptr<UiSystem>iconUi = icon->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/AllyIcon.ui", Sprite::SpriteShader::DEFALT, false);
        iconUi->LoadTexture(name);
    }

    //味方のネットの名前
    {
        std::shared_ptr<GameObject> hpFrame = GameObjectManager::Instance().Find("AllyHpFrame");
        std::shared_ptr<GameObject> playerName = hpFrame->AddChildObject();
        playerName->SetName("AllyPlayerName");
        std::shared_ptr<Font> font = playerName->AddComponent<Font>("Data/Texture/Font/BitmapFont.font", 1024);
        font->scale = 0.5f;
        font->color = { 1,1,1,1.0f };
        font->str = UTF8ToWString3(netPlayer.lock()->GetComponent<CharacterCom>()->GetNetCharaData().GeNetName());  //L付けてね
        font->position = { 139.0f,763.0f };
    }

    //electro
    {
        std::shared_ptr<GameObject> hpFrame = GameObjectManager::Instance().Find("AllyHpFrame");
        std::shared_ptr<GameObject> electro = hpFrame->AddChildObject();
        electro->SetName("Allyelectro");
        std::shared_ptr<Sprite>e = electro->AddComponent<Sprite>("Data/SerializeData/UIData/Player/Allyelectro.ui", Sprite::SpriteShader::DEFALT, false);
        e->SetColumns(6);
        e->SetRows(2);
        e->SetFrameRate(20.2f);
    }

    //HPエフェクト
    {
        for (int i = 0; i < 9; i++)
        {
            std::shared_ptr<GameObject> hpgauge = GameObjectManager::Instance().Find("AllyHpGauge");
            std::shared_ptr<GameObject> hpEffect = hpgauge->AddChildObject();
            std::string name = "HpEffect_" + i;
            hpEffect->SetName(name.c_str());
            int gaugeTexSize = hpgauge->GetComponent<UiGauge>()->originalTexSize.x;
            std::shared_ptr<UI_HPEffect>gauge = hpEffect->AddComponent<UI_HPEffect>("Data/SerializeData/UIData/Player/HpEffect.ui", Sprite::SpriteShader::DEFALT, true, gaugeTexSize, player, i);
        }
    }
}

void PlayerUIManager::CreateGameJudgeUI(PVPGameSystem::TEAM_KIND victryTeam)
{
    //一度だけ通る
    isEndFLG = true;
    //Judge
    {
        std::shared_ptr<GameObject> canvas = GameObjectManager::Instance().Find("Canvas");
        std::shared_ptr<GameObject> hit = canvas->AddChildObject();
        hit->SetName("GameJudge");
        //勝利したチーム番号を引数で送る
        hit->AddComponent<UI_GameJudge>(victryTeam);
    }
}

void PlayerUIManager::BookingRegistrationUI(std::shared_ptr<GameObject> obj)
{
    player = obj;
    bookingRegister = true;
}

UI_SkillComp::UI_SkillComp(CharacterCom::SkillCoolID skillid)
{
    skill = skillid;
}

void UI_SkillComp::Update(float elapsedTime)
{
    std::shared_ptr<GameObject> player = GameObjectManager::Instance().Find("player");
    auto& playerchara = player->GetComponent<CharacterCom>();
    GetGameObject()->GetComponent<UiSystem>()->SetEnabled(playerchara->IsSkillJustCooled(skill, 0.2f));

    //コマ数初期化
    if (!playerchara->GetCoolFlag(skill))
    {
        auto& s = GetGameObject()->GetComponent<Sprite>();
        s->SetColumns(10);
        s->SetRows(1);
    }
}

void UI_DeathComp::Update(float elapsedTime)
{
    std::shared_ptr<GameObject> player = GameObjectManager::Instance().Find("player");
    auto& playerstatus = player->GetComponent<CharaStatusCom>();
    auto& sprite = GetGameObject()->GetComponent<Sprite>();
    auto& charaicon = GameObjectManager::Instance().Find("CharaIcon")->GetComponent<UiSystem>();

    //起動
    sprite->SetEnabled(playerstatus->IsDeath());

    //イージング発動
    if (playerstatus->IsDeathFrame())
    {
        sprite->EasingPlay();
        charaicon->spc.color = { 1,1,1,0.4f };
    }

    //イージング停止
    if (!sprite->GetEnabled())
    {
        sprite->spc.scale = { 0.3f,0.3f };
        sprite->spc.color = { 1,1,1,1 };
        charaicon->spc.color = { 1,1,1,1 };
    }
}

UI_KillEffect::UI_KillEffect()
{
    std::shared_ptr<GameObject> Skull = GameObjectManager::Instance().Create();
    Skull->SetName("killSkull");
    Skull->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/KillEffect.ui", Sprite::SpriteShader::DEFALT, false);
}

void UI_KillEffect::Start()
{
    //親子付け
    this->GetGameObject()->AddChildObject(GameObjectManager::Instance().Find("killSkull"));
}

void UI_KillEffect::Update(float elapsedTime)
{
    effectFLGTimer -= elapsedTime;

    auto& player = GameObjectManager::Instance().Find("player")->GetComponent<CharacterCom>();
    for (int i = 0; i < 4; i++)
    {
        auto& killflg = StaticSendDataManager::Instance().GetKillID(player->GetNetCharaData().GetNetPlayerID(), i);
        if (killflg)
        {
            if (effectFLGTimer < 0)
            {
                effectFLG = true;
                effectFLGTimer = 3;
            }
        }
    }
    EffectUpdat(elapsedTime);
}

void UI_KillEffect::EffectUpdat(float elapsedTime)
{
    std::shared_ptr<UiSystem> skull = GameObjectManager::Instance().Find("killSkull")->GetComponent<UiSystem>();
    if (effectFLG) {
        effectFLG = false;
        skull->EasingPlay();
    }
    if (!skull->IsPlayEasing()) {
    }
}