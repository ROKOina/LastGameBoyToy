#pragma once

#include "Graphics/constant_buffer.h"
#include <DirectXMath.h>
#include <memory>

//シーン
class Scene
{
public:
    Scene() {}
    virtual ~Scene() {}

    //初期化
    virtual void Initialize() = 0;

    //終了化
    virtual void Finalize() = 0;

    //更新処理
    virtual void Update(float elapsedTime) = 0;

    //描画処理
    virtual void Render() = 0;

    //viewportの設定
    void ViewPortInitialize();

    //シーンのコンスタントバッファの更新
    void ConstantBufferUpdate();

    //シーンのコンスタントバッファの初期化
    void ConstantBufferInitialize();

    //準備完了しているか
    bool IsReady()const { return ready; }

    //準備完了設定
    void SetReady() { ready = true; }

protected:

    //シーンのコンスタントバッファの構造体
    struct SceneConstants
    {
        DirectX::XMFLOAT4X4 view = {};
        DirectX::XMFLOAT4X4 projection = {};
        DirectX::XMFLOAT4X4 viewprojection = {};
        DirectX::XMFLOAT4X4 inverseview = {};
        DirectX::XMFLOAT4X4 inverseprojection = {};
        DirectX::XMFLOAT3 cameraposition = {};
        float dummy = {};
    };
    std::unique_ptr<constant_buffer<SceneConstants>>sc;

protected:
    bool ready = false;
};