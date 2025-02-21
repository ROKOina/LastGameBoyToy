#pragma once

// スプライト描画用コンポーネント
class SpriteCom :public Component
{
    friend class SpriteCollisionCom;
    friend class EventSpriteFadeBehavior;
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

    SpriteCom(const char* filename, SpriteShader spriteshader);
    ~SpriteCom() {}

    //名前設定
    const char* GetName() const override { return "SpriteCom"; }

    //描画
    void Render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection);

    void DrawStart();

    void Draw(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection);

    void DrawEnd();

    //imgui
    void OnGUI()override;

public:
    //前後判定（ゲームオブジェクト追加された時だけソートする）
    //数字が大きいほど前に来る
    void SetOrderinLayer(int num) { spc.orderinLayer = num; }
    int GetOrderinLayer() { return spc.orderinLayer; }
    void SetDrawFinish(std::function<void()> callBack) { drawEnd.emplace_back() = callBack; }
    void SetDrawStart(std::function<void()> callBack) { drawStart.emplace_back() = callBack; }
public:

    void LoadTexture(std::string filename);
    //シリアライズ
    void Serialize();

    // デシリアライズ
    void Deserialize(const char* filename);

private:
    //頂点構造体
    struct Vertex
    {
        DirectX::XMFLOAT3	position;
        DirectX::XMFLOAT4	color;
        DirectX::XMFLOAT2	texcoord;
    };

public:
    //保存するパラメータ
    struct SaveParameterCPU
    {
        std::string	filename = {};
        DirectX::XMFLOAT2 pivot = { 0.0f,0.0f };
        DirectX::XMFLOAT2 texPos = { 0.0f, 0.0f };
        DirectX::XMFLOAT2 texSize = { 0.0f, 0.0f };
        DirectX::XMFLOAT4 color = { 1.0f,1.0f,1.0f,1.0f };
        int orderinLayer = 0;
        int blend = 1;
        int depth = 1;
        bool clearDepthStencilEnable = false;
        bool showMaskGraphicEnable = false;

        template<class Archive>
        void serialize(Archive& archive, int version);
    };
private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader>			vertexShader_;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>			pixelShader_;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>			inputLayout_;
    Microsoft::WRL::ComPtr<ID3D11Buffer>				vertexBuffer_;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	shaderResourceView_;
    D3D11_TEXTURE2D_DESC texture2ddesc_ = {};
    SaveParameterCPU spc = {};

    std::vector <std::function<void()>> drawEnd;
    std::vector <std::function<void()>>	drawStart;
    std::string filename = "";
    COPY_VARIABLE()

#ifdef DEBUG
    Vector2 offsetPos{};
#endif // DEBUG

};