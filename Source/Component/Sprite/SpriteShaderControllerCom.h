#pragma once

// スプライトシェーダー更新用コンポーネント
class SpriteShaderControllerCom :public Component
{
public:
    SpriteShaderControllerCom();
    ~SpriteShaderControllerCom() {}

    const char* GetName() const override { return "SpriteShaderControllerCom"; }

    //初期設定
    void Start()override;

    //imgui
    void OnGUI()override;

    //名前設定
    //クリップタイムセッター、ゲッター
    void SetClipTime(float cliptime) { m_constants.cliptime = cliptime; }
    float GetClipTime() { return m_constants.cliptime; }

    //アニメーションの列数と行数
    void SetColumns(int column) { m_constants.columns = column; }
    void SetRows(int row) { m_constants.rows = row; }

    //再生時間の係数(初期値が0なので自分で設定する)
    void SetFrameRate(float frame) { m_constants.framerate = frame; }
private:
    //描画開始用
    void DrawStart();

public:
    //定数バッファの構造体
    struct SaveConstantsParameter
    {
        DirectX::XMFLOAT3 blurcolor = { 1.0f,1.0f,1.0f };
        float blurdistance = { 8.0f };
        float blurpower = { 0.1f };
        float luminance = { 0.0f };
        DirectX::XMFLOAT2 uvscroll = { 0.0f,0.0f };
        DirectX::XMFLOAT3 edgecolor = { 1,1,1 };
        float cliptime = 0.0f;
        float edgethreshold = 1.0f;
        float edgeoffset = 0.0f;
        int onflag = 1;
        float framerate = { 0.0f };
        int columns = { 1 };
        int rows = { 1 };
        DirectX::XMFLOAT2 padding = {};
    };

private:
    SaveConstantsParameter m_constants;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantbuffer;
};