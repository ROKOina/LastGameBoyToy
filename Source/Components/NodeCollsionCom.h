#pragma once

#include "System\Component.h"
#include "Graphics\Model\Model.h"

//アニメーション
class NodeCollsionCom : public Component
{
    //コンポーネントオーバーライド
public:
    NodeCollsionCom(const char* filename);
    ~NodeCollsionCom() {}

    // 名前取得
    const char* GetName() const override { return "NodeCollsion"; }

    // 開始処理
    void Start() override;

    // 更新処理
    void Update(float elapsedTime) override;

    // GUI描画
    void OnGUI() override;

private:

    //デバッグプリミティブの形状
    enum class CollsionType
    {
        SPHER,
        CYLINDER,
        MAX
    };
    CollsionType m_collsiontype = CollsionType::SPHER;

private:

    const char* m_filename = {};
    Model* model = nullptr;
    bool debugrender = false;
};