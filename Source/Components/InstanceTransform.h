#pragma once

#include "System\Component.h"
#include "Graphics\Model\Model.h"

class InstanceTransform : public Component
{
    //コンポーネントオーバーライド
public:
    InstanceTransform() {};
    ~InstanceTransform() {}

    // 名前取得
    const char* GetName() const override { return "InstanceTransform"; }

    // 開始処理
    void Start() override {};

    // 更新処理
    void Update(float elapsedTime) override;

    // GUI描画
    void OnGUI() override {};

private:

    //インスタンシングの情報
    struct Instance
    {
        DirectX::XMFLOAT4 quaternion = { 0,0,0,1 };
        DirectX::XMFLOAT3 position{ 0,0,0 };
        DirectX::XMFLOAT3 scale{ 1.0f,1.0f,1.0f };
    };
    std::unique_ptr<Instance[]> m_cpuinstancedata;
};