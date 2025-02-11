#pragma once
#include "SpriteCom.h"

// スプライト描画用コンポーネント
class SpriteCollisionCom :public Component
{
public:

    SpriteCollisionCom(std::shared_ptr<SpriteCom> spr);
    ~SpriteCollisionCom() {}

    //初期設定
    void Start()override;

    //更新処理
    void Update(float elapsedTime)override;

    //描画
    void Render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection);

    //imgui
    void OnGUI()override;

    //名前設定
    const char* GetName() const override { return "SpriteCollisionCom"; }

private:

    //当たり判定用短形
    void DrawCollsionBox();

    //マウスカーソルとコリジョンボックスの当たり判定
    bool cursorVsCollsionBox();

public:

    //カーソルがスプライトに当たっているか
    bool GetHitSpriteCollisionCom() { return hit; }
    void SetHitSpriteCollisionCom(bool h) { hit = h; }

    //カーソルがスプライトに当たった瞬間
    bool GetHitSpriteCollisionComEnter() { return hitEnter; }

private:

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	collsionshaderResourceView_;
    D3D11_TEXTURE2D_DESC collisionTexture2ddesc_ = {};

    std::weak_ptr<SpriteCom> spr;
    bool drawcollsion = false;
    bool hit = false;
    bool hitEnter = false;
    bool ontriiger = false;

    DirectX::XMFLOAT2 collisionPivot;
};