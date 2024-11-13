#pragma once

#include "Component\System\Component.h"

class Light :public Component
{
public:

    Light(const char* filename);
    ~Light() {};

    //初期設定
    void Start()override {};

    //更新処理
    void Update(float elapsedTime)override;

    //imgui
    void OnGUI()override;

    //名前設定
    const char* GetName() const override { return "Light"; }

public:

    // 光源タイプ
    enum class LightType
    {
        Directional,	// 平行光源
        Point,			// 点光源
        Spot,			// スポットライト
    };
};