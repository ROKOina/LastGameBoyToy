#pragma once
#include "SpriteCom.h"

// スプライト当たり判定用コンポーネント
class SpriteCollisionCom :public Component
{
    friend class SpriteCom;
public:
    SpriteCollisionCom();
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

    void SetHitSprite(bool h) { hit = h; }
public:

    //カーソルがスプライトに当たっているか
    bool GetHitSprite() { return hit; }

    //カーソルがスプライトに当たった瞬間
    void SetHitSpriteEnter(std::function<void()> callBack) { hitEnter.emplace_back() = callBack; }

    // カーソルがスプライトから離れた瞬間
    void SetHitSpriteExit(std::function<void()> callBack) { hitExit.emplace_back() = callBack; }

    void SetOffsetScale(Vector2 scale) { offsetScale = scale; }
    void SetOffsetScale(float scale) { offsetScale.x = offsetScale.y = scale; }
private:

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	collsionshaderResourceView_;
    D3D11_TEXTURE2D_DESC collisionTexture2ddesc_ = {};

    std::weak_ptr<SpriteCom> spr;
    bool drawcollsion = false;
    bool hit = false;

    bool ontriiger = true;

    std::vector <std::function<void()>> hitEnter;
    std::vector <std::function<void()>>	hitExit;

    Vector2 offsetScale = { 0,0 };
};