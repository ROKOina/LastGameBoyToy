#include "PlayerUI.h"
#include "Math\Mathf.h"
#include "Component\Character\CharacterCom.h"

UI_Skill::UI_Skill(const char* filename, SpriteShader spriteshader, bool collsion, float min, float max) :UiSystem(filename, spriteshader, collsion)
{
    changePosValue = min - max;
    //�����}�C�i�X�Ȃ琮���l�ɕς���
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
    //�Q�[�W�̔{�������߂�
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
        //�u�[�X�g�񐔂���UI�𑝂₷
        //<Frame>//
        std::shared_ptr<GameObject> BoostFrame = GameObjectManager::Instance().Create();
        std::string name = "BoostFrame_";
        std::string number = std::to_string(i);
        name += number;
        std::shared_ptr<UiSystem> frame = BoostFrame->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/BoostFrame.ui", Sprite::SpriteShader::DEFALT, false);
        BoostFrame->SetName(name.c_str());
        //�ʒu����
        frame->spc.position = { frame->spc.position.x + (i * (frame->spc.texSize.x * frame->spc.scale.x)) ,frame->spc.position.y - i * 19.0f };

        //<Gauge//
        std::shared_ptr<GameObject> BoostGauge = GameObjectManager::Instance().Create();
        name = "BoostGauge_";
        name += number;
        BoostGauge->SetName(name.c_str());
        std::shared_ptr<UiSystem> gauge = BoostGauge->AddComponent<UiSystem>("Data/SerializeData/UIData/Player/BoostGauge.ui", Sprite::SpriteShader::DEFALT, false);
        gauge->SetVariableValue(player.lock()->GetComponent<CharacterCom>()->GetDashGauge());
        //�ʒu����
        gauge->spc.position = { gauge->spc.position.x + (i * (gauge->spc.texSize.x * gauge->spc.scale.x)) ,gauge->spc.position.y - i * 19.0f };

        originlTexSize = gauge->spc.texSize;
        frames.emplace_back(BoostFrame);
        gauges.emplace_back(BoostGauge);
    }

    this->num = num;
    maxDashGauge = player.lock()->GetComponent<CharacterCom>()->GetDahsGaugeMax();
    value = player.lock()->GetComponent<CharacterCom>()->GetDashGauge();

    //��؂�̒l�����߂�
    separateValue = maxDashGauge / num;
}

void UI_BoosGauge::Start()
{
    //�Ȃ���initialize�ł͐e�q�t���ł��Ȃ�����
    //�e�q�t��
    for (int i = 0; i < num; i++) {
        this->GetGameObject()->AddChildObject(frames.at(i));
        this->GetGameObject()->AddChildObject(gauges.at(i));
    }
}

void UI_BoosGauge::Update(float elapsedTime)
{
    std::shared_ptr<UiSystem> gauge = gauges.at(0).get()->GetComponent<UiSystem>();
    std::shared_ptr<UiSystem> gauge2 = gauges.at(1).get()->GetComponent<UiSystem>();

    // �ω��l���}�C�i�X�ɍs���Ȃ��悤�ɕ␳
    *value = Mathf::Clamp(*value, 0.01f, maxDashGauge);
    float valueRate;
    //�Q�[�W�̔{�������߂�
    valueRate = *value / (maxDashGauge);

    for (int i = 0; i < num; i++)
    {
        std::shared_ptr<UiSystem> gaugeSegment = gauges.at(i).get()->GetComponent<UiSystem>();
        float segmentStart = separateValue * i;       // ���݂̃Q�[�W�̊J�n�ʒu
        float segmentEnd = separateValue * (i + 1);   // ���݂̃Q�[�W�̏I���ʒu

        if (*value > segmentEnd) {
            // �Z�O�����g���t���̏ꍇ
            gaugeSegment->spc.texSize = { originlTexSize.x, gaugeSegment->spc.texSize.y };
        }
        else if (*value > segmentStart) {
            // �Z�O�����g�������I�ɖ��܂��Ă���ꍇ
            float partialRate = (*value - segmentStart) / separateValue; // �Z�O�����g���̔䗦
            gaugeSegment->spc.texSize = { originlTexSize.x * partialRate, gaugeSegment->spc.texSize.y };
        }
        else {
            // �Z�O�����g����̏ꍇ
            gaugeSegment->spc.texSize = { 0, gaugeSegment->spc.texSize.y };
        }
    }
}

UI_LockOn::UI_LockOn(int num)
{
    for (int i = 0; i < num; i++) {
        //���A�N�^�[��ێ�
        std::string name = "Reactar";
        name = name + std::to_string(i);
        reacters.emplace_back(GameObjectManager::Instance().Find(name.c_str()));
        similarity.emplace_back(0.0f);
    }
    //�J�����ێ�
    camera = GameObjectManager::Instance().Find("cameraPostPlayer");
    //Ui�̃Q�[���I�u�W�F�N�g����
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
    //�J�����̃|�W�V����
    DirectX::XMFLOAT3 cameraPos = camera.lock()->GetComponent<CameraCom>()->GetEye();

    //�J�����̃x�N�g��
    DirectX::XMFLOAT3 cameraVec = camera.lock()->GetComponent<CameraCom>()->GetFocus() - cameraPos;
    cameraVec = Mathf::Normalize(cameraVec);
    //�ǂ͈̔͂܂ŕ`�悷�邩��臒l
    float threshold = 0.99f;

    std::shared_ptr<GameObject> nearReacter;
    float maxSimilarity = 0.0f;
    int i = 0;
    for (auto& reacter : reacters) {
        DirectX::XMFLOAT3 reacterPos = reacter->GetComponent<TransformCom>()->GetWorldPosition();
        DirectX::XMFLOAT3 reacterDirection = reacterPos - cameraPos;
        reacterDirection = Mathf::Normalize(reacterDirection);

        //1�ɋ߂��قǎ����������Ă���
        similarity.at(i) = Mathf::Dot(cameraVec, reacterDirection);

        //臒l�𒴂��Ă��邩�����钆�ň�ԋ߂���
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