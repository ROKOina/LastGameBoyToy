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

private:

    //シリアライズ
    void Serialize();

    // デシリアライズ
    void Deserialize(const char* filename);

    // デシリアライズの読み込み
    void LoadDeserialize();

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

public:

    //staticコリジョンのパラメータ
    struct StaticCollsionParameter
    {
        int collsiontype = {};
        float radius = 0.0f;
        DirectX::XMFLOAT3 scale = { 0,0,0 };
        DirectX::XMFLOAT3 offsetpos = { 0,0,0 };

        template<class Archive>
        void serialize(Archive& archive, int version);
    };
    std::unordered_map<int, std::vector<StaticCollsionParameter>>scp;

private:

    const char* m_filename = {};
    Model* model = nullptr;
    bool debugrender = false;
    CollsionType m_collsiontype = CollsionType::SPHER;
};