#pragma once

#include "StateMachine/StateMachine.h"
#include "Audio\Audio3D.h"

class StageGimmick : public Component
{
public:

    StageGimmick() {};
    ~StageGimmick() {};

    //初期設定
    void Start()override;

    //更新処理
    void Update(float elapsedTime)override;

    //imgui
    void OnGUI()override;

    //名前設定
    const char* GetName() const override { return "StageGimmick"; }

public:

    //ステート
    enum class GimmickState
    {
        IDLE,
        ENEMYSPAWN,
        BIGATTACK,
        BREAK,
        MAX
    };

    StateMachine<StageGimmick, GimmickState>& GetStateMachine() { return state; }

private:

    StateMachine<StageGimmick, GimmickState> state;

private://音
    std::map<AUDIOID3D, GameObj> audios;
public:
    void SetAudios(AUDIOID3D id, GameObj obj) {
        GameObj SeObj = obj->AddChildObject();
        std::shared_ptr<AudioSource3D> Se = SeObj->AddComponent<AudioSource3D>(id);
        Se->SetCurveDistanceScaler(70.0f);
        audios[id] = SeObj;
    }
    AudioSource3D* GetAudios(AUDIOID3D id) { return audios[id]->GetComponent<AudioSource3D>().get(); }

};