#pragma once

#include "Component\System\Component.h"
#include "Graphics/ConstantBuffer.h"

class Trail :public Component
{
public:
    Trail(const char* filename);
    ~Trail() {}

    //初期設定
    void Start()override {};

    //更新処理
    void Update(float elapsedTime)override;

    //描画
    void Render();

    //imgui
    void OnGUI()override;

    //名前設定
    const char* GetName() const override { return "Trail"; }

    //トレイルに必用な行列と始点と終点のオフセット値を代入する関数
    void SetTransform(const DirectX::XMFLOAT4X4& worldtransform, const DirectX::XMFLOAT3& tail, const DirectX::XMFLOAT3& root);
    void SetTransform(const DirectX::XMFLOAT4X4& worldtransform);

public:

    //保存する構造体
    struct TrailParameter
    {
        std::string filename = {};
        DirectX::XMFLOAT3 rootpos = {};
        DirectX::XMFLOAT3 tailpos = {};
        float lifetime = 1.0f;
        int blend = 1;

        template<class Archive>
        void serialize(Archive& archive, int version);
    };
    TrailParameter TP;

    //トレイルの定数バッファ(保存する)
    struct TrailConstants
    {
        DirectX::XMFLOAT4 trailcolor = { 1,1,1,1 };
        DirectX::XMFLOAT3 trailcolorscale = { 1,1,1 };
        float padding = {};
        DirectX::XMFLOAT2 uvscroll = {};
        DirectX::XMFLOAT2 padding2 = {};

        template<class Archive>
        void serialize(Archive& archive, int version);
    };
    TrailConstants TC;

private:

    //シリアライズ
    void Serialize();

    //デシリアライズ
    void Desirialize(const char* filename);

    //読み込み
    void LoadDesirialize();

    //頂点追加
    void AddVertex(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& texcoord, float lifetime);

private:

    //頂点の構造体
    struct Vertex
    {
        DirectX::XMFLOAT3	position = {};
        DirectX::XMFLOAT2	texcoord = {};
        DirectX::XMFLOAT4	color = { 1,1,1,1 };
        float lifetime = 0.0f;
    };
    std::vector<Vertex> m_vertices;

private:

    std::string filepath = {};
    DirectX::XMMATRIX m_worldtransform = {};
    static const UINT VertexCapacity = 1024;
    static const int MAX_POLYGON = 32;
    DirectX::XMFLOAT3 m_trailpositions[2][MAX_POLYGON]{};
    Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_vertexshader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_pixelshader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_inputlayout;
    Microsoft::WRL::ComPtr<ID3D11Buffer>		m_vertexbuffer;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>m_trailresource;
    Microsoft::WRL::ComPtr<ID3D11Buffer>m_constantbuffer;
};