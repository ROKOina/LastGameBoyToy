#pragma once

#include "Components/System/Component.h"

class EasingMoveCom :public Component
{
public:

    EasingMoveCom(const char* filename);
    ~EasingMoveCom() {};

    //初期設定
    void Start()override;

    //更新処理
    void Update(float elapsedTime)override;

    //imgui
    void OnGUI()override;

    //名前設定
    const char* GetName() const override { return "EasingMove"; }

private:

    //イージング停止
    void StopEasing();

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
        float timescale = 0.0f;
        int easingtype = 0;
        int easingmovetype = 0;
        bool loop = false;
        bool comback = false;
        DirectX::XMFLOAT3 easingposition = {};
        DirectX::XMFLOAT3 easingscale = {};
        std::string objectname = {};

        template<class Archive>
        void serialize(Archive& archive, int version);
    };
    EasingMoveParameter EMP = {};

private:

    float easingresult = 0.0f;
    float easingtime = 0.0f;
    bool play = false;
    bool loop = false;
    bool one = false;
    DirectX::XMFLOAT3 savepos = {};
    DirectX::XMFLOAT3 savescale = {};
};