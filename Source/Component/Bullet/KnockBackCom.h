#pragma once

#include "../System\Component.h"

class KnockBackCom : public Component
{
public:
    KnockBackCom() {};
    ~KnockBackCom() override {};

    // 名前取得
    const char* GetName() const override { return "KnockBack"; }

    // 開始処理
    void Start() override {};

    // 更新処理
    void Update(float elapsedTime) override;

    // GUI描画
    void OnGUI() override {};

public:

    //ノックバックの係数
    void SetKnockBackForce(DirectX::XMFLOAT3& knockbackforce_) { knockbackforce = knockbackforce_; }

private:

    DirectX::XMFLOAT3 knockbackforce = {};
};