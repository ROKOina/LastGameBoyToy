#pragma once

#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include "Components/System/Component.h"
#include "Components/CameraCom.h"

// スプライト
class Sprite :public Component
{
public:

    Sprite(const char* filename, bool collsion);
    ~Sprite() {}

    //初期設定
    void Start()override {};

    //更新処理
    void Update(float elapsedTime)override;

    //描画
    void Render();

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
        DirectX::XMFLOAT4 color = { 1,1,1,1 };
        DirectX::XMFLOAT4 easingcolor = { 1,1,1,1 };
        DirectX::XMFLOAT2 position = {};
        DirectX::XMFLOAT2 easingposition = {};
        DirectX::XMFLOAT2 scale = {};
        DirectX::XMFLOAT2 easingscale = {};
        DirectX::XMFLOAT2 collsionscaleoffset = {};
        DirectX::XMFLOAT2 collsionpositionoffset = {};
        float angle = {};
        float easingangle = {};
        std::string	filename;
        int blend = 1;
        int depth = 1;
        float timescale = 0.0f;
        int easingtype = 0;
        int easingmovetype = 0;
        bool loop = false;
        bool comback = false;

        template<class Archive>
        void serialize(Archive& archive, int version);
    };
    SaveParameterCPU spc;

    //定数バッファの構造体
    struct SaveConstantsParameter
    {
        DirectX::XMFLOAT2 uvscroll = { 0.0f,0.0f };
        float cliptime = 0.0f;
        float edgethreshold = 0.6f;
        float edgeoffset = 0.1f;
        DirectX::XMFLOAT3 edgecolor = { 1,1,1 };
        DirectX::XMFLOAT4X4 world = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
    };
    SaveConstantsParameter constants;

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
    DirectX::XMFLOAT2 savepos = {};
    DirectX::XMFLOAT4 savecolor = {};
    DirectX::XMFLOAT2 savescale = {};
    float saveangle = {};
    float easingresult = 0.0f;
    float easingtime = 0.0f;
    bool play = false;
    bool loopon = false;
    bool drawcollsion = false;
    bool hit = false;
    bool ontriiger = false;
    bool easingsprite = false;
};