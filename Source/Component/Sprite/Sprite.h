#pragma once

#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include "Component/System/Component.h"
#include "Component/Camera/CameraCom.h"

// スプライト
class Sprite :public Component
{
public:

    //シェーダーコードを変更する
    enum class SpriteShader
    {
        DEFALT,
        DISSOLVE
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

    //名前設定
    const char* GetName() const override { return "Sprite"; }

private:

    //シリアライズ
    void Serialize();

    // デシリアライズ
    void Deserialize(const char* filename);

    // デシリアライズの読み込み
    void LoadDeserialize();

    //イージング停止
    void StopEasing();

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
        DirectX::XMFLOAT2 texSize{ 0.0f,0.0f };
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

        template<class Archive>
        void serialize(Archive& archive, int version);
    };
    SaveParameterCPU spc = {};

    //定数バッファの構造体
    struct SaveConstantsParameter
    {
        DirectX::XMFLOAT2 uvscroll = { 0.0f,0.0f };
        float cliptime = 0.0f;
        float edgethreshold = 0.6f;
        float edgeoffset = 0.1f;
        DirectX::XMFLOAT3 edgecolor = { 1,1,1 };
    };
    SaveConstantsParameter constants;

    //カーソルがスプライトに当たっているか
    bool GetHitSprite() { return hit; }

    //数字用に追加
    DirectX::XMFLOAT2 numUVScroll = { 0,0 };

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
    bool ontriiger = false;
};