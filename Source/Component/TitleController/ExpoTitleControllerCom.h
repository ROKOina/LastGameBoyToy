#pragma once
#include <Component\Event\EventDirectCom.h>
#include <Component\Sprite\SpriteCollisionCom.h>

// タイトル制御用コンポーネント
class ExpoTitleControllerCom :public Component
{
public:

    ExpoTitleControllerCom();
    ~ExpoTitleControllerCom() {}

    //名前設定
    const char* GetName() const override { return "ExpoTitleControllerCom"; }

    //初期設定
    void Start()override;

    //更新処理
    void Update(float elapsedTime)override;

    void CreditUpdate();

    void SelectUpdate();

    void UIActive(bool enable);

    //imgui
    void OnGUI()override;

    void RegisterUIObj(std::shared_ptr<GameObject> ui) { uiObjs.emplace_back(ui); }
    void SetIntroUIEvent(std::shared_ptr<EventDirectCom> introEvent) { introUiEvent = introEvent; }
    void RegisterSelectEvent(std::shared_ptr<EventDirectCom> selectEvent) { selectEvents.emplace_back(selectEvent); }
public:
    enum TitleState
    {
        INTRO,
        SELECT_UPDATE,
        CREDIT_UPDATE,
        NEXT_SCENE,
    };

    enum SelectItem
    {
        PVE,
        PVP,
        TRAINING,
        CREDIT,
        END_GAME,
        NONE,
    };
private:
    std::vector<std::weak_ptr<GameObject>> uiObjs;
    std::weak_ptr<EventDirectCom> introUiEvent;
    std::vector<std::weak_ptr<EventDirectCom>>selectEvents;

    TitleState state = TitleState::INTRO;
    SelectItem selectState = SelectItem::NONE;
    std::vector<std::weak_ptr<SpriteCollisionCom>>uiCollisions;
    std::vector<std::weak_ptr<GameObject>> selectObjs;
};