#pragma once

#include "Component\System\Component.h"
#include "Graphics\Video.h"
#include "Graphics/ConstantBuffer.h"


class Video :public Component
{
public:
    Video(const char* filename);
    ~Video();

    //初期設定
    void Start()override {};

    //更新処理
    void Update(float elapsedTime)override;

    //描画
    void Render();

    //imgui
    void OnGUI()override;

    //名前設定
    const char* GetName() const override { return "Video"; }

    VideoTexture GetVidePram() { return video; }

private:

    //頂点バッファの構造体
    struct Vertex
    {
        float   x, y, z;
        float   u, v;
        DirectX::XMFLOAT4 color;
    };
    std::vector<Vertex> vtxs;

    //定数バッファ
    struct VideoConstants
    {
        DirectX::XMFLOAT4X4 world = {};
    };
    std::unique_ptr<ConstantBuffer<VideoConstants>>VC;

private:
    VideoTexture video;
    Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_vertexshader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_pixelshader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_inputlayout;
    Microsoft::WRL::ComPtr<ID3D11Buffer>	    vertexbuffer;
};