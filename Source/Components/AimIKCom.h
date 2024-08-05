#pragma once

#include "System\Component.h"
#include "Graphics\Model\Model.h"

//aimikcom
class AimIKCom : public Component
{
    //コンポーネントオーバーライド
public:
    AimIKCom(const char* ainbonename);
    ~AimIKCom() {}

    // 名前取得
    const char* GetName() const override { return "AimIK"; }

    // 開始処理
    void Start() override;

    // 更新処理
    void Update(float elapsedTime) override;

    // GUI描画
    void OnGUI() override {};

private:

    //AimIK関数
    void AimIK();

    //ikさせるboneを探す
    void SearchAimNode(const char* ainbonename);

private:

    //AimIK用変数
    std::vector<int>AimBone;
    const char* copyname = {};
};