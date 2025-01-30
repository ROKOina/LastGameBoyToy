#pragma once
#include "Component\System\Component.h"
#include "Component\System\GameObject.h"

class JumpBoardCom : public Component
{
public:
    JumpBoardCom() {};
    ~JumpBoardCom() {};

    // 名前取得
    const char* GetName() const override { return "JumpBoard"; }

    // 開始処理
    void Start() {}

    // 更新処理
    void Update(float elapsedTime);

    // 破棄処理
    void OnDestroy();

    // GUI描画
    void OnGUI();

private:

};
