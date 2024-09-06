#include <stdio.h>
#include <WICTextureLoader.h>
#include "Sprite.h"
#include "Misc.h"
#include "Graphics/Graphics.h"
#include "Graphics/Shaders/Shader.h"
#include "Graphics/Shaders/Texture.h"
#include "Dialog.h"
#include "Logger.h"
#include "GameSource/Math/Mathf.h"
#include <imgui.h>
#include <fstream>
#include <filesystem>
#include <shlwapi.h>
#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>
#include "GameSource/Math/easing.h"
#include "Components/TransformCom.h"
#include <Input/Input.h>

CEREAL_CLASS_VERSION(Sprite::SaveParameterCPU, 1)

// シリアライズ
namespace DirectX
{
    template<class Archive>
    void serialize(Archive& archive, XMUINT4& v)
    {
        archive(
            cereal::make_nvp("x", v.x),
            cereal::make_nvp("y", v.y),
            cereal::make_nvp("z", v.z),
            cereal::make_nvp("w", v.w)
        );
    }

    template<class Archive>
    void serialize(Archive& archive, XMFLOAT2& v)
    {
        archive(
            cereal::make_nvp("x", v.x),
            cereal::make_nvp("y", v.y)
        );
    }

    template<class Archive>
    void serialize(Archive& archive, XMFLOAT3& v)
    {
        archive(
            cereal::make_nvp("x", v.x),
            cereal::make_nvp("y", v.y),
            cereal::make_nvp("z", v.z)
        );
    }

    template<class Archive>
    void serialize(Archive& archive, XMFLOAT4& v)
    {
        archive(
            cereal::make_nvp("x", v.x),
            cereal::make_nvp("y", v.y),
            cereal::make_nvp("z", v.z),
            cereal::make_nvp("w", v.w)
        );
    }

    template<class Archive>
    void serialize(Archive& archive, XMFLOAT4X4& m)
    {
        archive(
            cereal::make_nvp("_11", m._11), cereal::make_nvp("_12", m._12), cereal::make_nvp("_13", m._13), cereal::make_nvp("_14", m._14),
            cereal::make_nvp("_21", m._21), cereal::make_nvp("_22", m._22), cereal::make_nvp("_23", m._23), cereal::make_nvp("_24", m._24),
            cereal::make_nvp("_31", m._31), cereal::make_nvp("_32", m._32), cereal::make_nvp("_33", m._33), cereal::make_nvp("_34", m._34),
            cereal::make_nvp("_41", m._41), cereal::make_nvp("_42", m._42), cereal::make_nvp("_43", m._43), cereal::make_nvp("_44", m._44)
        );
    }
}

template<class Archive>
void Sprite::SaveParameterCPU::serialize(Archive& archive, int version)
{
    archive
    (
        CEREAL_NVP(color),
        CEREAL_NVP(easingcolor),
        CEREAL_NVP(position),
        CEREAL_NVP(easingposition),
        CEREAL_NVP(scale),
        CEREAL_NVP(easingscale),
        CEREAL_NVP(collsionscaleoffset),
        CEREAL_NVP(collsionpositionoffset),
        CEREAL_NVP(angle),
        CEREAL_NVP(easingangle),
        CEREAL_NVP(filename),
        CEREAL_NVP(blend),
        CEREAL_NVP(depth),
        CEREAL_NVP(timescale),
        CEREAL_NVP(easingtype),
        CEREAL_NVP(easingmovetype),
        CEREAL_NVP(loop),
        CEREAL_NVP(comback)
    );
}

// コンストラクタ
Sprite::Sprite(const char* filename, bool collsion)
{
    HRESULT hr = S_OK;

    ID3D11Device* device = Graphics::Instance().GetDevice();
    ID3D11DeviceContext* dc = Graphics::Instance().GetDeviceContext();

    // 頂点初期化
    Vertex vertices[]
    {
        { { -1.0f, +1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f } },
        { { +1.0f, +1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f } },
        { { -1.0f, -1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f } },
        { { +1.0f, -1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f } },
    };

    // テクスチャの情報を設定する
    D3D11_BUFFER_DESC buffer_desc{};
    buffer_desc.ByteWidth = sizeof(vertices);
    buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
    buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    D3D11_SUBRESOURCE_DATA subresource_data{};
    subresource_data.pSysMem = vertices;
    hr = device->CreateBuffer(&buffer_desc, &subresource_data, vertexBuffer_.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

    //コンスタントバッファのバッファ作成
    {
        D3D11_BUFFER_DESC desc;
        ::memset(&desc, 0, sizeof(desc));
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;
        desc.ByteWidth = sizeof(SaveConstantsParameter);
        desc.StructureByteStride = 0;
        hr = device->CreateBuffer(&desc, nullptr, m_constantbuffer.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
    }

    // 頂点シェーダー
    {
        //入力レイアウト
        D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        CreateVsFromCso(device, "Shader\\SpriteVS.cso", vertexShader_.GetAddressOf(), inputLayout_.GetAddressOf(), inputElementDesc, _countof(inputElementDesc));
    }

    // ピクセルシェーダー
    {
        CreatePsFromCso(device, "Shader\\SpritePS.cso", pixelShader_.GetAddressOf());
    }

    //ファイル読み込み処理
    if (filename)
    {
        Deserialize(filename);
        LoadTextureFromFile(device, spc.filename.c_str(), shaderResourceView_.GetAddressOf(), &texture2ddesc_);
    }

    //コリジョンデータ読み込み
    if (collsion)
    {
        LoadTextureFromFile(device, "Data\\Texture\\collsionbox.png", collsionshaderResourceView_.GetAddressOf(), &texture2ddesc_);
    }

    //Dissolveデータ読み込み
    LoadTextureFromFile(device, "Data\\Texture\\noise.png", noiseshaderresourceview_.GetAddressOf(), &texture2ddesc_);
    LoadTextureFromFile(device, "Data\\Texture\\Ramp.png", rampshaderresourceview_.GetAddressOf(), &texture2ddesc_);
    LoadTextureFromFile(device, "Data\\Texture\\easing.png", easingshaderresourceview_.GetAddressOf(), &texture2ddesc_);

    //コリジョンを使うか決める
    ontriiger = collsion;
}

//更新処理
void Sprite::Update(float elapsedTime)
{
    // イージングが有効な場合
    if (play)
    {
        //イージング更新
        easingresult = EasingUpdate(spc.easingtype, spc.easingmovetype, easingtime);

        // イージング計算
        spc.position = Mathf::Lerp(savepos, spc.easingposition, easingresult);
        spc.color = Mathf::Lerp(savecolor, spc.easingcolor, easingresult);
        spc.scale = Mathf::Lerp(savescale, spc.easingscale, easingresult);
        spc.angle = Mathf::Lerp(saveangle, spc.easingangle, easingresult);

        // イージング時間の更新
        easingtime += (loopon ? -1.0f : 1.0f) * elapsedTime * spc.timescale;

        // イージング時間の範囲チェック
        if (easingtime > 1.0f)
        {
            // ループまたは戻り値が有効な場合
            if (spc.loop || spc.comback)
            {
                loopon = !loopon;
                easingtime = 1.0f;
            }
            else
            {
                StopEasing(); // イージングを停止
            }
        }
        else if (easingtime < 0.0f)
        {
            // ループが有効な場合
            if (spc.loop)
            {
                loopon = !loopon;
                easingtime = 0.0f;
            }
            else
            {
                StopEasing(); // イージングを停止
            }
        }
    }

    //当たり判定
    if (ontriiger)
    {
        if (cursorVsCollsionBox())
        {
            hit = true;
        }
        else
        {
            hit = false;
        }
    }
}

//描画
void Sprite::Render()
{
    Graphics& Graphics = Graphics::Instance();
    ID3D11Device* device = Graphics.GetDevice();
    ID3D11DeviceContext* dc = Graphics.GetDeviceContext();

    //viewport設定
    D3D11_VIEWPORT viewport{};
    UINT num_viewports{ 1 };
    dc->RSGetViewports(&num_viewports, &viewport);

    //ステートの設定
    dc->OMSetBlendState(Graphics.GetBlendState(static_cast<BLENDSTATE>(spc.blend)), nullptr, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(Graphics.GetDepthStencilState(static_cast<DEPTHSTATE>(spc.depth)), 1);
    dc->RSSetState(Graphics.GetRasterizerState(RASTERIZERSTATE::SOLID_CULL_NONE));

    // スプライトの頂点座標をスクリーン空間に設定
    float x0{ spc.position.x - spc.scale.x * 0.5f };
    float y0{ spc.position.y - spc.scale.y * 0.5f };
    float x1{ spc.position.x + spc.scale.x * 0.5f };
    float y1{ spc.position.y - spc.scale.y * 0.5f };
    float x2{ spc.position.x - spc.scale.x * 0.5f };
    float y2{ spc.position.y + spc.scale.y * 0.5f };
    float x3{ spc.position.x + spc.scale.x * 0.5f };
    float y3{ spc.position.y + spc.scale.y * 0.5f };

    auto rotate = [](float& x, float& y, float cx, float cy, float angle)
        {
            x -= cx;
            y -= cy;

            float cos{ cosf(DirectX::XMConvertToRadians(angle)) };
            float sin{ sinf(DirectX::XMConvertToRadians(angle)) };
            float tx{ x }, ty{ y };
            x = cos * tx - sin * ty;
            y = sin * tx + cos * ty;

            x += cx;
            y += cy;
        };
    float cx = spc.position.x;
    float cy = spc.position.y;
    rotate(x0, y0, cx, cy, spc.angle);
    rotate(x1, y1, cx, cy, spc.angle);
    rotate(x2, y2, cx, cy, spc.angle);
    rotate(x3, y3, cx, cy, spc.angle);

    // NDC空間への変換
    x0 = 2.0f * x0 / viewport.Width - 1.0f;
    y0 = 1.0f - 2.0f * y0 / viewport.Height;
    x1 = 2.0f * x1 / viewport.Width - 1.0f;
    y1 = 1.0f - 2.0f * y1 / viewport.Height;
    x2 = 2.0f * x2 / viewport.Width - 1.0f;
    y2 = 1.0f - 2.0f * y2 / viewport.Height;
    x3 = 2.0f * x3 / viewport.Width - 1.0f;
    y3 = 1.0f - 2.0f * y3 / viewport.Height;

    HRESULT hr{ S_OK };
    D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
    hr = dc->Map(vertexBuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

    Vertex* vertices{ reinterpret_cast<Vertex*>(mapped_subresource.pData) };
    if (vertices != nullptr)
    {
        vertices[0].position = { x0, y0 , 0.0f };
        vertices[1].position = { x1, y1 , 0.0f };
        vertices[2].position = { x2, y2 , 0.0f };
        vertices[3].position = { x3, y3 , 0.0f };

        vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = { spc.color.x, spc.color.y, spc.color.z, spc.color.w };

        vertices[0].texcoord = { 1.0f / texture2ddesc_.Width, 1.0f / texture2ddesc_.Height };
        vertices[1].texcoord = { (0.0f + static_cast<float>(texture2ddesc_.Width)) / texture2ddesc_.Width, 0.0f / texture2ddesc_.Height };
        vertices[2].texcoord = { 0.0f / texture2ddesc_.Width, (0.0f + static_cast<float>(texture2ddesc_.Height)) / texture2ddesc_.Height };
        vertices[3].texcoord = { (0.0f + static_cast<float>(texture2ddesc_.Width)) / texture2ddesc_.Width, (0.0f + static_cast<float>(texture2ddesc_.Height)) / texture2ddesc_.Height };
    }
    dc->Unmap(vertexBuffer_.Get(), 0);

    //設定
    UINT stride{ sizeof(Vertex) };
    UINT offset{ 0 };
    dc->IASetVertexBuffers(0, 1, vertexBuffer_.GetAddressOf(), &stride, &offset);
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    dc->VSSetConstantBuffers(0, 1, m_constantbuffer.GetAddressOf());
    dc->PSSetConstantBuffers(0, 1, m_constantbuffer.GetAddressOf());
    dc->UpdateSubresource(m_constantbuffer.Get(), 0, 0, &constants, 0, 0);
    dc->IASetInputLayout(inputLayout_.Get());
    dc->VSSetShader(vertexShader_.Get(), nullptr, 0);
    dc->PSSetShader(pixelShader_.Get(), nullptr, 0);
    dc->PSSetShaderResources(0, 1, shaderResourceView_.GetAddressOf());
    dc->PSSetShaderResources(1, 1, noiseshaderresourceview_.GetAddressOf());
    dc->PSSetShaderResources(2, 1, rampshaderresourceview_.GetAddressOf());
    dc->Draw(4, 0);

    //当たり判定可視化
    if (drawcollsion && ontriiger)
    {
        DrawCollsionBox();
    }

    //いーじんぐのスプライト描画
    if (easingsprite)
    {
        EasingSprite();
    }
}

//当たり判定用短形
void Sprite::DrawCollsionBox()
{
    Graphics& graphics = Graphics::Instance();
    ID3D11Device* device = graphics.GetDevice();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();

    // スプライトと同じビューポートの設定
    D3D11_VIEWPORT viewport{};
    UINT num_viewports{ 1 };
    dc->RSGetViewports(&num_viewports, &viewport);

    // スプライトの位置とスケールにオフセット値を加算
    float x0{ spc.position.x + spc.collsionpositionoffset.x - (spc.scale.x + spc.collsionscaleoffset.x) * 0.5f };
    float y0{ spc.position.y + spc.collsionpositionoffset.y - (spc.scale.y + spc.collsionscaleoffset.y) * 0.5f };
    float x1{ spc.position.x + spc.collsionpositionoffset.x + (spc.scale.x + spc.collsionscaleoffset.x) * 0.5f };
    float y1{ spc.position.y + spc.collsionpositionoffset.y - (spc.scale.y + spc.collsionscaleoffset.y) * 0.5f };
    float x2{ spc.position.x + spc.collsionpositionoffset.x - (spc.scale.x + spc.collsionscaleoffset.x) * 0.5f };
    float y2{ spc.position.y + spc.collsionpositionoffset.y + (spc.scale.y + spc.collsionscaleoffset.y) * 0.5f };
    float x3{ spc.position.x + spc.collsionpositionoffset.x + (spc.scale.x + spc.collsionscaleoffset.x) * 0.5f };
    float y3{ spc.position.y + spc.collsionpositionoffset.y + (spc.scale.y + spc.collsionscaleoffset.y) * 0.5f };

    auto rotate = [](float& x, float& y, float cx, float cy, float angle)
        {
            x -= cx;
            y -= cy;

            float cos{ cosf(DirectX::XMConvertToRadians(angle)) };
            float sin{ sinf(DirectX::XMConvertToRadians(angle)) };
            float tx{ x }, ty{ y };
            x = cos * tx - sin * ty;
            y = sin * tx + cos * ty;

            x += cx;
            y += cy;
        };

    float cx = spc.position.x + spc.collsionpositionoffset.x;
    float cy = spc.position.y + spc.collsionpositionoffset.y;
    rotate(x0, y0, cx, cy, spc.angle);
    rotate(x1, y1, cx, cy, spc.angle);
    rotate(x2, y2, cx, cy, spc.angle);
    rotate(x3, y3, cx, cy, spc.angle);

    // NDC空間への変換
    x0 = 2.0f * x0 / viewport.Width - 1.0f;
    y0 = 1.0f - 2.0f * y0 / viewport.Height;
    x1 = 2.0f * x1 / viewport.Width - 1.0f;
    y1 = 1.0f - 2.0f * y1 / viewport.Height;
    x2 = 2.0f * x2 / viewport.Width - 1.0f;
    y2 = 1.0f - 2.0f * y2 / viewport.Height;
    x3 = 2.0f * x3 / viewport.Width - 1.0f;
    y3 = 1.0f - 2.0f * y3 / viewport.Height;

    // 当たり判定用の矩形を描画するための頂点
    Vertex collisionVertices[]
    {
        { { x0, y0, 0.0f }, { 1.0f, 0.0f, 0.0f, 0.3f }, { 0.0f, 0.0f } }, // 赤色で表示
        { { x1, y1, 0.0f }, { 1.0f, 0.0f, 0.0f, 0.3f }, { 1.0f, 0.0f } },
        { { x2, y2, 0.0f }, { 1.0f, 0.0f, 0.0f, 0.3f }, { 0.0f, 1.0f } },
        { { x3, y3, 0.0f }, { 1.0f, 0.0f, 0.0f, 0.3f }, { 1.0f, 1.0f } },
    };

    // 当たり判定用の矩形を描画するための頂点バッファの作成
    D3D11_BUFFER_DESC buffer_desc{};
    buffer_desc.ByteWidth = sizeof(collisionVertices);
    buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
    buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    D3D11_SUBRESOURCE_DATA subresource_data{};
    subresource_data.pSysMem = collisionVertices;
    ID3D11Buffer* collisionVertexBuffer = nullptr;
    HRESULT hr = device->CreateBuffer(&buffer_desc, &subresource_data, &collisionVertexBuffer);
    if (FAILED(hr)) {
        LOG("当たり判定用頂点バッファの作成に失敗しました。HRESULT: 0x%X", hr);
        return; // エラー処理
    }

    // 描画設定
    UINT stride{ sizeof(Vertex) };
    UINT offset{ 0 };
    dc->IASetVertexBuffers(0, 1, &collisionVertexBuffer, &stride, &offset);
    dc->PSSetShaderResources(0, 1, collsionshaderResourceView_.GetAddressOf());

    // スプライトの矩形を描画
    dc->Draw(4, 0);

    // メモリの解放
    if (collisionVertexBuffer) {
        collisionVertexBuffer->Release();
        collisionVertexBuffer = nullptr;
    }
}

//いーじんぐ
void Sprite::EasingSprite()
{
    Graphics& graphics = Graphics::Instance();
    ID3D11Device* device = graphics.GetDevice();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();

    // スプライトと同じビューポートの設定
    D3D11_VIEWPORT viewport{};
    UINT num_viewports{ 1 };
    dc->RSGetViewports(&num_viewports, &viewport);

    // スプライトの位置とスケールにオフセット値を加算
    float x0{ spc.easingposition.x - spc.easingscale.x * 0.5f };
    float y0{ spc.easingposition.y - spc.easingscale.y * 0.5f };
    float x1{ spc.easingposition.x + spc.easingscale.x * 0.5f };
    float y1{ spc.easingposition.y - spc.easingscale.y * 0.5f };
    float x2{ spc.easingposition.x - spc.easingscale.x * 0.5f };
    float y2{ spc.easingposition.y + spc.easingscale.y * 0.5f };
    float x3{ spc.easingposition.x + spc.easingscale.x * 0.5f };
    float y3{ spc.easingposition.y + spc.easingscale.y * 0.5f };

    auto rotate = [](float& x, float& y, float cx, float cy, float angle)
        {
            x -= cx;
            y -= cy;

            float cos{ cosf(DirectX::XMConvertToRadians(angle)) };
            float sin{ sinf(DirectX::XMConvertToRadians(angle)) };
            float tx{ x }, ty{ y };
            x = cos * tx - sin * ty;
            y = sin * tx + cos * ty;

            x += cx;
            y += cy;
        };

    float cx = spc.easingposition.x;
    float cy = spc.easingposition.y;
    rotate(x0, y0, cx, cy, spc.easingangle);
    rotate(x1, y1, cx, cy, spc.easingangle);
    rotate(x2, y2, cx, cy, spc.easingangle);
    rotate(x3, y3, cx, cy, spc.easingangle);

    // NDC空間への変換
    x0 = 2.0f * x0 / viewport.Width - 1.0f;
    y0 = 1.0f - 2.0f * y0 / viewport.Height;
    x1 = 2.0f * x1 / viewport.Width - 1.0f;
    y1 = 1.0f - 2.0f * y1 / viewport.Height;
    x2 = 2.0f * x2 / viewport.Width - 1.0f;
    y2 = 1.0f - 2.0f * y2 / viewport.Height;
    x3 = 2.0f * x3 / viewport.Width - 1.0f;
    y3 = 1.0f - 2.0f * y3 / viewport.Height;

    //当たり判定用の矩形を描画するための頂点
    Vertex easingVertices[]
    {
        { { x0, y0, 0.0f }, { spc.easingcolor }, { 1.0f / texture2ddesc_.Width, 1.0f / texture2ddesc_.Height } }, // 赤色で表示
        { { x1, y1, 0.0f }, { spc.easingcolor }, { (0.0f + static_cast<float>(texture2ddesc_.Width)) / texture2ddesc_.Width, 0.0f / texture2ddesc_.Height } },
        { { x2, y2, 0.0f }, { spc.easingcolor }, { 0.0f / texture2ddesc_.Width, (0.0f + static_cast<float>(texture2ddesc_.Height)) / texture2ddesc_.Height } },
        { { x3, y3, 0.0f }, { spc.easingcolor }, { (0.0f + static_cast<float>(texture2ddesc_.Width)) / texture2ddesc_.Width, (0.0f + static_cast<float>(texture2ddesc_.Height)) / texture2ddesc_.Height } },
    };

    // いーじんぐ用の矩形を描画するための頂点バッファの作成
    D3D11_BUFFER_DESC buffer_desc{};
    buffer_desc.ByteWidth = sizeof(easingVertices);
    buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
    buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    D3D11_SUBRESOURCE_DATA subresource_data{};
    subresource_data.pSysMem = easingVertices;
    ID3D11Buffer* easingVertexBuffer = nullptr;
    HRESULT hr = device->CreateBuffer(&buffer_desc, &subresource_data, &easingVertexBuffer);
    if (FAILED(hr)) {
        LOG("いーじんぐ用頂点バッファの作成に失敗しました。HRESULT: 0x%X", hr);
        return; // エラー処理
    }

    // 描画設定
    UINT stride{ sizeof(Vertex) };
    UINT offset{ 0 };
    dc->IASetVertexBuffers(0, 1, &easingVertexBuffer, &stride, &offset);
    dc->PSSetShaderResources(0, 1, easingshaderresourceview_.GetAddressOf());

    // スプライトの矩形を描画
    dc->Draw(4, 0);

    // メモリの解放
    if (easingVertexBuffer) {
        easingVertexBuffer->Release();
        easingVertexBuffer = nullptr;
    }
}

//imgui
void Sprite::OnGUI()
{
    // ファイルの保存と読み込み
    ImGui::Text("File Operations");
    ImGui::SameLine();
    if (ImGui::Button("Save"))
    {
        StopEasing();
        Serialize();
    }
    ImGui::SameLine();
    if (ImGui::Button("Load"))
    {
        LoadDeserialize();
        LoadTextureFromFile(Graphics::Instance().GetDevice(), spc.filename.c_str(), shaderResourceView_.GetAddressOf(), &texture2ddesc_);
    }

    // テクスチャの選択
    ImGui::Text("Texture Selection");
    char textureFile[256];
    ZeroMemory(textureFile, sizeof(textureFile));
    ::strncpy_s(textureFile, sizeof(textureFile), spc.filename.c_str(), sizeof(textureFile));
    if (ImGui::Button("..."))
    {
        const char* filter = "Texture Files(*.DDS;*.dds;*.png;*.jpg;)\0*.DDS;*.dds;*.png;*.jpg;\0All Files(*.*)\0*.*;\0\0";
        DialogResult result = Dialog::OpenFileName(textureFile, sizeof(textureFile), filter, nullptr, Graphics::Instance().GetHwnd());
        if (result == DialogResult::OK)
        {
            std::filesystem::path path = std::filesystem::current_path();
            path.append("Data");

            char drive[32], dir[256], dirname[256];
            ::_splitpath_s(path.string().c_str(), drive, sizeof(drive), dir, sizeof(dir), nullptr, 0, nullptr, 0);
            ::_makepath_s(dirname, sizeof(dirname), drive, dir, nullptr, nullptr);
            dirname[strlen(dirname) - 1] = '\0';
            char relativeTextureFile[MAX_PATH];
            PathRelativePathToA(relativeTextureFile, dirname, FILE_ATTRIBUTE_DIRECTORY, textureFile, FILE_ATTRIBUTE_ARCHIVE);

            // 読み込み
            spc.filename = relativeTextureFile;
            LoadTextureFromFile(Graphics::Instance().GetDevice(), spc.filename.c_str(), shaderResourceView_.GetAddressOf(), &texture2ddesc_);
        }
    }
    ImGui::SameLine();
    ::strncpy_s(textureFile, sizeof(textureFile), spc.filename.c_str(), sizeof(textureFile));
    if (ImGui::InputText("texture", textureFile, sizeof(textureFile), ImGuiInputTextFlags_EnterReturnsTrue))
    {
        spc.filename = textureFile;

        char drive[32], dir[256], fullPath[256];
        ::_splitpath_s(textureFile, drive, sizeof(drive), dir, sizeof(dir), nullptr, 0, nullptr, 0);
        ::_makepath_s(fullPath, sizeof(fullPath), drive, dir, textureFile, nullptr);
        LoadTextureFromFile(Graphics::Instance().GetDevice(), spc.filename.c_str(), shaderResourceView_.GetAddressOf(), &texture2ddesc_);
    }

    // テクスチャのプレビュー
    ImGui::Text("Resource Preview");
    ImGui::Image(shaderResourceView_.Get(), { 256, 256 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });

    // アニメーション制御
    ImGui::Text("Animation Controls");
    if (ImGui::Button("Play") && !play)
    {
        savepos = spc.position;
        savecolor = spc.color;
        savescale = spc.scale;
        saveangle = spc.angle;
        play = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Pause"))
    {
        StopEasing();
    }

    // イージング設定
    ImGui::Text("Easing Settings");
    easingresult = EasingImGui(spc.easingtype, spc.easingmovetype, easingtime);
    ImGui::Checkbox("EasingSprite", &easingsprite);

    // ブレンドモード設定
    constexpr const char* BlendName[] =
    {
        "NONE", "ALPHA", "ADD", "SUBTRACT", "REPLACE",
        "MULTIPLY", "LIGHTEN", "DARKEN", "SCREEN", "MULTIPLERENDERTARGETS",
    };
    ImGui::Combo("BlendMode", &spc.blend, BlendName, static_cast<int>(BLENDSTATE::MAX), 10);

    // デプスステンシルモード設定
    constexpr const char* DepthName[] =
    {
        "NONE", "ZT_ON_ZW_ON", "ZT_ON_ZW_OFF", "ZT_OFF_ZW_ON", "ZT_OFF_ZW_OFF",
        "SILHOUETTE", "MASK", "APPLY_MASK", "EXCLUSIVE",
    };
    ImGui::Combo("DepthMode", &spc.depth, DepthName, static_cast<int>(DEPTHSTATE::MAX), static_cast<int>(DEPTHSTATE::MAX));

    // その他のパラメータ
    ImGui::Text("Parameters");

    if (ImGui::TreeNode("ScreennPos"))
    {
        ImGui::TreePop();
    }

    //コンスタントバッファ
    if (ImGui::TreeNode("ConstantsBuffer"))
    {
        ImGui::DragFloat2((char*)u8"UVスクロール", &constants.uvscroll.x, 0.1f);
        ImGui::DragFloat((char*)u8"クリップ", &constants.cliptime, 0.1f, 0.0f, 1.0f);
        ImGui::DragFloat((char*)u8"縁しきい値", &constants.edgethreshold, 0.1f, 0.0f, 1.0f);
        ImGui::DragFloat((char*)u8"縁オフセット値", &constants.edgeoffset, 0.1f, 0.0f, 1.0f);
        ImGui::ColorEdit3((char*)u8"縁色", &constants.edgecolor.x);
        ImGui::TreePop();
    }

    //コリジョンパラメータ
    if (ImGui::TreeNode("Collsion"))
    {
        ImGui::DragFloat2((char*)u8"位置オフセット値", &spc.collsionpositionoffset.x);
        ImGui::DragFloat2((char*)u8"スケールオフセット値", &spc.collsionscaleoffset.x);
        ImGui::Checkbox((char*)u8"当たり判定描画", &drawcollsion);
        ImGui::TreePop();
    }

    //動きのパラメータ
    if (ImGui::TreeNode("SpriteParameter"))
    {
        ImGui::ColorEdit4((char*)u8"色", &spc.color.x);
        ImGui::ColorEdit4((char*)u8"イージング色", &spc.easingcolor.x);
        ImGui::DragFloat2((char*)u8"位置", &spc.position.x);
        ImGui::DragFloat2((char*)u8"イージング位置", &spc.easingposition.x);
        ImGui::DragFloat2((char*)u8"大きさ", &spc.scale.x);
        ImGui::DragFloat2((char*)u8"イージング大きさ", &spc.easingscale.x);
        ImGui::DragFloat((char*)u8"回転", &spc.angle);
        ImGui::DragFloat((char*)u8"イージング回転", &spc.easingangle);
        ImGui::DragFloat((char*)u8"再生速度", &spc.timescale, 0.1f, 0.0f, 5.0f);
        ImGui::TreePop();
    }

    ImGui::Checkbox((char*)u8"ループ再生", &spc.loop);
    ImGui::SameLine();
    ImGui::Checkbox((char*)u8"ワンカット再生", &spc.comback);
    ImGui::SameLine();
    if (ontriiger)
    {
        ImGui::Checkbox((char*)u8"ヒット！", &hit);
    }
}

//シリアライズ
void Sprite::Serialize()
{
    static const char* filter = "UI Files(*.ui)\0*.ui;\0All Files(*.*)\0*.*;\0\0";

    char filename[256] = { 0 };
    DialogResult result = Dialog::SaveFileName(filename, sizeof(filename), filter, nullptr, "ui", Graphics::Instance().GetHwnd());
    if (result == DialogResult::OK)
    {
        std::ofstream ostream(filename, std::ios::binary);
        if (ostream.is_open())
        {
            cereal::BinaryOutputArchive archive(ostream);

            try
            {
                archive
                (
                    CEREAL_NVP(spc)
                );
            }
            catch (...)
            {
                LOG("ui deserialize failed.\n%s\n", filename);
                return;
            }
        }
    }
}

//デシリアライズ
void Sprite::Deserialize(const char* filename)
{
    std::ifstream istream(filename, std::ios::binary);
    if (istream.is_open())
    {
        cereal::BinaryInputArchive archive(istream);

        try
        {
            archive
            (
                CEREAL_NVP(spc)
            );
        }
        catch (...)
        {
            LOG("ui deserialize failed.\n%s\n", filename);
            return;
        }
    }
}

//読み込み
void Sprite::LoadDeserialize()
{
    static const char* filter = "UI Files(*.ui)\0*.ui;\0All Files(*.*)\0*.*;\0\0";

    char filename[256] = { 0 };
    DialogResult result = Dialog::OpenFileName(filename, sizeof(filename), filter, nullptr, Graphics::Instance().GetHwnd());
    if (result == DialogResult::OK)
    {
        Deserialize(filename);
    }
}

//イージング停止
void Sprite::StopEasing()
{
    play = false;
    loopon = false;
    easingtime = 0.0f;
    spc.position = savepos;
    spc.color = savecolor;
    spc.scale = savescale;
    spc.angle = saveangle;
}

//マウスカーソルとコリジョンボックスの当たり判定
bool Sprite::cursorVsCollsionBox()
{
    //マウスの位置
    Mouse& mouse = Input::Instance().GetMouse();
    float mousePosx = mouse.GetPositionX();
    float mousePosy = mouse.GetPositionY();

    //法線ベクトル作成
    float x{ sinf(DirectX::XMConvertToRadians(spc.angle)) };
    float y{ cosf(DirectX::XMConvertToRadians(spc.angle)) };
    DirectX::XMFLOAT2 normalUp = Mathf::Normalize({ x,y });
    DirectX::XMFLOAT3 normalCross = Mathf::Cross({ normalUp.x,normalUp.y,0 }, { 0,0,1 });
    DirectX::XMFLOAT2 normalRight = Mathf::Normalize({ normalCross.x,normalCross.y });

    //カーソル位置からコリジョンボックスのベクトル
    DirectX::XMFLOAT2 cur = { mousePosx ,mousePosy };
    DirectX::XMFLOAT2 pos = { spc.position.x + spc.collsionpositionoffset.x ,spc.position.y + spc.collsionpositionoffset.y };
    DirectX::XMFLOAT2 curVecPos = cur - pos;
    curVecPos.y *= -1;

    //長さを測る
    float upLen = Mathf::Dot(normalUp, curVecPos);
    float rightLen = Mathf::Dot(normalRight, curVecPos);

    //判定
    DirectX::XMFLOAT2 scale = { (spc.scale.x + spc.collsionscaleoffset.x) * 0.5f ,(spc.scale.y + spc.collsionscaleoffset.y) * 0.5f };
    if (upLen * upLen > scale.y * scale.y)return false;
    if (rightLen * rightLen > scale.x * scale.x)return false;

    return true;
}