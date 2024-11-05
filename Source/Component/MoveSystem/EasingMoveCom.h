#pragma once

#include "Component/System/Component.h"

class EasingMoveCom :public Component
{
public:

    EasingMoveCom(const char* filename);
    ~EasingMoveCom() {};

    //初期設定
    void Start()override {};

    //更新処理
    void Update(float elapsedTime)override;

    //imgui
    void OnGUI()override;

    //名前設定
    const char* GetName() const override { return "EasingMove"; }

    // 終了時のコールバック関数
    std::function<void()> onFinishCallback = nullptr;

private:

    //イージング停止
    void StopEasing();

    //オブジェクトがあれば
    void Object();

    //シリアライズ
    void Serialize();

    // デシリアライズ
    void Deserialize(const char* filename);

    // デシリアライズの読み込み
    void LoadDeserialize();

public:

    struct EasingMoveParameter
    {
        std::string	filename = {};
        float delaytime = {};
        bool delatimeuse = false;
        std::vector<float> timescale = {};
        std::vector<int> easingtype = {};
        std::vector<int> easingmovetype = {};
        std::vector<DirectX::XMFLOAT3> easingposition = {};
        std::string objectname = {};
        float trackingtime = 0.0f;
        bool deleteflag = false;
        float deletetime = 0.0f;

        template<class Archive>
        void serialize(Archive& archive, int version);
    };
    EasingMoveParameter EMP = {};

private:

    float easingresult = 0.0f;
    float easingtime = 0.0f;
    float time = 0.0f;
    float deletetime = 0.0f;
    bool play = false;
    bool stop = false;
    DirectX::XMFLOAT3 savepos = {};
    size_t currentTargetIndex = 0; // 現在の目標ポイントのインデックス
    std::string filepath = {};
};