#pragma once

#include <wrl.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include "Components/System/Component.h"

// スプライト
class Sprite :public Component
{
public:

    Sprite(const char* filename);
    ~Sprite() {}

    //初期設定
    void Start()override {};

    //更新処理
    void Update(float elapsedTime)override {};

    //描画
    void Render();

    //imgui
    void OnGUI()override;

    //名前設定
    const char* GetName() const override { return "Sprite"; }

private:

    //シリアライズ
    void Serialize();

    //デシリアライズ
    void Desirialize(const char* filename);

    //読み込み
    void LoadDesirialize();

public:

    //保存するパラメータ
    struct SaveParameterCPU
    {
        DirectX::XMFLOAT4 color = { 1,1,1,1 };
        DirectX::XMFLOAT2 position = {};
        DirectX::XMFLOAT2 scale = {};
        float angle = {};
        std::string	filename;

        template<class Archive>
        void serialize(Archive& archive, int version);
    };
    SaveParameterCPU spc;

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
    D3D11_TEXTURE2D_DESC texture2ddesc_ = {};
};