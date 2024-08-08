#include <stdio.h>
#include <WICTextureLoader.h>
#include "Sprite.h"
#include "Misc.h"
#include "Graphics/Graphics.h"
#include "Graphics/Shaders/Shader.h"
#include "Graphics/Shaders/Texture.h"
#include "Dialog.h"
#include "Logger.h"
#include <imgui.h>
#include <fstream>
#include <filesystem>
#include <shlwapi.h>
#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>

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
        CEREAL_NVP(position),
        CEREAL_NVP(scale),
        CEREAL_NVP(angle),
        CEREAL_NVP(filename)
    );
}

// コンストラクタ
Sprite::Sprite(const char* filename)
{
    HRESULT hr = S_OK;

    ID3D11Device* device = Graphics::Instance().GetDevice();

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
        Desirialize(filename);
        LoadTextureFromFile(device, spc.filename.c_str(), shaderResourceView_.GetAddressOf(), &texture2ddesc_);
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

    // ステートの設定
    dc->OMSetBlendState(Graphics.GetBlendState(BLENDSTATE::ALPHA), nullptr, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(Graphics.GetDepthStencilState(DEPTHSTATE::ZT_ON_ZW_ON), 1);
    dc->RSSetState(Graphics.GetRasterizerState(RASTERIZERSTATE::SOLID_CULL_NONE));

    // スプライトの頂点座標をスクリーン空間に設定
    float x0{ spc.position.x };
    float y0{ spc.position.y };
    float x1{ spc.position.x + spc.scale.x };
    float y1{ spc.position.y };
    float x2{ spc.position.x };
    float y2{ spc.position.y + spc.scale.y };
    float x3{ spc.position.x + spc.scale.x };
    float y3{ spc.position.y + spc.scale.y };

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
    float cx = spc.position.x + spc.scale.x * 0.5f;
    float cy = spc.position.y + spc.scale.y * 0.5f;
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

        vertices[0].texcoord = { 0.0f / texture2ddesc_.Width, 0.0f / texture2ddesc_.Height };
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
    dc->IASetInputLayout(inputLayout_.Get());
    dc->VSSetShader(vertexShader_.Get(), nullptr, 0);
    dc->PSSetShader(pixelShader_.Get(), nullptr, 0);
    dc->PSSetShaderResources(0, 1, shaderResourceView_.GetAddressOf());
    dc->Draw(4, 0);
}

//imgui
void Sprite::OnGUI()
{
    ImGui::SameLine();
    if (ImGui::Button("Save"))
    {
        Serialize();
    }
    ImGui::SameLine();
    if (ImGui::Button("Load"))
    {
        LoadDesirialize();
        LoadTextureFromFile(Graphics::Instance().GetDevice(), spc.filename.c_str(), shaderResourceView_.GetAddressOf(), &texture2ddesc_);
    }
    //テクスチャロード
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
    ImGui::Text((char*)u8"リソース");
    ImGui::Image(shaderResourceView_.Get(), { 256, 256 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
    ImGui::ColorEdit4((char*)u8"色", &spc.color.x);
    ImGui::DragFloat2((char*)u8"位置", &spc.position.x);
    ImGui::DragFloat2((char*)u8"大きさ", &spc.scale.x);
    ImGui::DragFloat((char*)u8"回転", &spc.angle);
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
void Sprite::Desirialize(const char* filename)
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
void Sprite::LoadDesirialize()
{
    static const char* filter = "UI Files(*.ui)\0*.ui;\0All Files(*.*)\0*.*;\0\0";

    char filename[256] = { 0 };
    DialogResult result = Dialog::OpenFileName(filename, sizeof(filename), filter, nullptr, Graphics::Instance().GetHwnd());
    if (result == DialogResult::OK)
    {
        Desirialize(filename);
    }
}