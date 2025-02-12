#pragma once

#include "Component/Camera/CameraCom.h"

// スプライト
class Sprite :public Component
{
public:

    //シェーダーコードを変更する
    enum class SpriteShader
    {
        DEFALT,
        DEFALTUV,
        BLUR,
        DISSOLVE,
        CHROMATICABERRATION,
        GLITCH,
        HOLO,
        ELECTRO,
        MAX
    };

public:

    Sprite(const char* filename, SpriteShader spriteshader, bool collsion);
    ~Sprite() {}

    //初期設定
    void Start()override;

    //更新処理
    void Update(float elapsedTime)override;

    //描画
    void Render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection);

    //imgui
    void OnGUI()override;

    void LoadTexture(std::string filename);

    //名前設定
    const char* GetName() const override { return "Sprite"; }

    //イージングプレイ関数
    void EasingPlay();

    //イージング停止
    void StopEasing();

    //イージング再生中か
    bool IsPlayEasing() { return play; }
    //イージング時間
    float GetEasingTime() { return easingtime; }
    void GetEasingTimeReset() { easingtime = 0; }

private:

    //シリアライズ
    void Serialize();

    // デシリアライズ
    void Deserialize(const char* filename);

    // デシリアライズの読み込み
    void LoadDeserialize();

    //当たり判定用短形
    void DrawCollsionBox();

    //マウスカーソルとコリジョンボックスの当たり判定
    bool cursorVsCollsionBox();

    //いーじんぐ
    void EasingSprite();

public:

    //保存するパラメータ
    struct SaveParameterCPU
    {
        DirectX::XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f };
        DirectX::XMFLOAT4 easingcolor = { 1.0f,1.0f,1.0f,1.0f };
        DirectX::XMFLOAT2 position = { 0.0f,0.0f };
        DirectX::XMFLOAT2 easingposition = { 0.0f,0.0f };
        DirectX::XMFLOAT2 scale = { 0.0f,0.0f };
        DirectX::XMFLOAT2 easingscale = { 0.0f,0.0f };
        DirectX::XMFLOAT2 collsionscaleoffset = { 0.0f,0.0f };
        DirectX::XMFLOAT2 collsionpositionoffset = { 0.0f,0.0f };
        DirectX::XMFLOAT2 pivot = { 0.0f,0.0f };
        DirectX::XMFLOAT2 texSize = { 0.0f,0.0f };
        float angle = { 0.0f };
        float easingangle = { 0.0f };
        std::string	filename = {};
        int blend = 1;
        int depth = 1;
        float timescale = 0.0f;
        int easingtype = 0;
        int easingmovetype = 0;
        bool loop = false;
        bool comback = false;
        bool easing = false;
        std::string objectname = {};
        DirectX::XMFLOAT3 screenposoffset = { 0.0f,0.0f,0.0f };
        DirectX::XMFLOAT2 maxscale = { 0.0f,0.0f };
        DirectX::XMFLOAT2 minscale = { 0.0f,0.0f };
        bool onshot = false;

        template<class Archive>
        void serialize(Archive& archive, int version);
    };
    SaveParameterCPU spc = {};

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
    SaveConstantsParameter constants;

    //カーソルがスプライトに当たっているか
    bool GetHitSprite() { return hit; }
    void SetHitSprite(bool h) { hit = h; }
    //カーソルがスプライトに当たった瞬間
    bool GetHitSpriteEnter() { return hitEnter; }

    //クリップタイムセッター、ゲッター
    void SetClipTime(float cliptime) { constants.cliptime = cliptime; }
    float GetClipTime() { return constants.cliptime; }

    //アニメーションの列数と行数
    void SetColumns(int column) { constants.columns = column; }
    void SetRows(int row) { constants.rows = row; }

    //再生時間の係数(初期値が0なので自分で設定する)
    void SetFrameRate(float frame) { constants.framerate = frame; }

    //数字用に追加
    DirectX::XMFLOAT2 numUVScroll = { 0,0 };

    //前後判定（ゲームオブジェクト追加された時だけソートする）
    void SetOrderinLayer(int num) { orderinLayer = num; }
    int GetOrderinLayer() { return orderinLayer; }

    //親子関係？
    void SetIsParentMove(bool flag) { isParentMove = flag; }
    void SetParentPosOffset(DirectX::XMFLOAT3 pos) { parentPosOffset.x = pos.x; parentPosOffset.y = pos.y; }

    //イージング位置を動的変更
    void SetEasingPosition(DirectX::XMFLOAT2 easingpos) { spc.easingposition = easingpos; }

private:

    //頂点構造体
    struct Vertex
    {
        DirectX::XMFLOAT3	position;
        DirectX::XMFLOAT4	color;
        DirectX::XMFLOAT2	texcoord;
    };

private:

    Microsoft::WRL::ComPtr<ID3D11VertexShader>			vertexShader_;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>			pixelShader_;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>			inputLayout_;
    Microsoft::WRL::ComPtr<ID3D11Buffer>				vertexBuffer_;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	shaderResourceView_;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	collsionshaderResourceView_;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	noiseshaderresourceview_;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	rampshaderresourceview_;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	easingshaderresourceview_;
    Microsoft::WRL::ComPtr<ID3D11Buffer>                m_constantbuffer;
    D3D11_TEXTURE2D_DESC texture2ddesc_ = {};
    D3D11_TEXTURE2D_DESC collisionTexture2ddesc_ = {};
    D3D11_TEXTURE2D_DESC rampTexture2ddesc_ = {};
    D3D11_TEXTURE2D_DESC dissolveTexture2ddesc_ = {};
    DirectX::XMFLOAT2 savepos = {};
    DirectX::XMFLOAT4 savecolor = { 1,1,1,1 };
    DirectX::XMFLOAT2 savescale = {};
    float saveangle = {};
    float easingresult = 0.0f;
    float easingtime = 0.0f;
    bool play = false;
    bool loopon = false;
    bool drawcollsion = false;
    bool hit = false;
    bool hitEnter = false;
    bool ontriiger = false;

    //シリアライズのファイルパス
    std::string filepath = {};

    //数字が大きいほど前に来る
    int orderinLayer = 0;

    DirectX::XMFLOAT2 collisionPivot;

    bool isParentMove = false;
    DirectX::XMFLOAT3 parentPosOffset = {};
};