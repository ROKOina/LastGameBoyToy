#pragma once

#include "Component\System\Component.h"
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

public:

    //デバッグプリミティブの形状
    enum class CollsionType
    {
        SPHER,
        CYLINDER,
        BOX,
        MAX
    };

    //部位毎のenumclass
    enum class BosyPartType
    {
        NONE,
        BODY,
        FACE,
        RIGHTHAND,
        LEFTHAND,
        RIGHTLEG,
        LEFTLEG,
        MAX
    };

    CollsionType GetColiisionType() { return m_collsiontype; }

private:

    CollsionType m_collsiontype = CollsionType::SPHER;

private:

    const char* m_filename = {};
    Model* model = nullptr;
    bool debugrender = false;
};