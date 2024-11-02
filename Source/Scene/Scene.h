#pragma once

#include "Graphics/ConstantBuffer.h"
#include <DirectXMath.h>
#include <memory>
#include <string>

//前方宣言
class CameraCom;

//シーン
class Scene
{
private:
    // コピーコンストラクタ防止
    Scene(const Scene&) = delete;
    Scene(Scene&&)  noexcept = delete;
    Scene& operator=(Scene const&) = delete;
    Scene& operator=(Scene&&) noexcept = delete;

public:
    Scene();
    virtual ~Scene() {}

    //初期化
    virtual void Initialize() = 0;

    //終了化
    virtual void Finalize() = 0;

    //更新処理
    virtual void Update(float elapsedTime) = 0;

    //描画処理
    virtual void Render(float elapsedTime) = 0;

    //名前取得
    virtual std::string GetName() const = 0;

    //準備完了しているか
    bool IsReady()const { return isReady_; }

    //準備完了設定
    void SetReady() { isReady_ = true; }

    //シーンのコンスタントバッファの更新
    void ConstantBufferUpdate(float elapsedTime);

    //シーンのコンスタントバッファの初期化
    void ConstantBufferInitialize();

protected:
    //シーンのコンスタントバッファの構造体
    struct SceneConstants
    {
        DirectX::XMFLOAT4X4 view = {};
        DirectX::XMFLOAT4X4 projection = {};
        DirectX::XMFLOAT4X4 viewprojection = {};
        DirectX::XMFLOAT4X4 inverseview = {};
        DirectX::XMFLOAT4X4 inverseprojection = {};
        DirectX::XMFLOAT4X4 inverseviewprojection = {};
        DirectX::XMFLOAT3 cameraposition = {};
        float time = {};
        DirectX::XMFLOAT3 bossposiotn = {};
        float deltatime = {};
        DirectX::XMFLOAT2 cameraScope = {};
        DirectX::XMFLOAT2 screenResolution = {};
    };
    std::unique_ptr<ConstantBuffer<SceneConstants>>sc;

private:
    bool isReady_ = false;
};