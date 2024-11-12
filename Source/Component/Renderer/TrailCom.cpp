#include "TrailCom.h"
#include "Graphics\Shader.h"
#include "Graphics\Graphics.h"
#include "Graphics\Texture.h"
#include "Component\System\TransformCom.h"
#include "Math\Mathf.h"
#include <fstream>
#include <filesystem>
#include <shlwapi.h>
#include <SystemStruct\Dialog.h>
#include "SystemStruct\Logger.h"
#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>
#include <random>
#include "Component\Camera\CameraCom.h"

CEREAL_CLASS_VERSION(Trail::TrailParameter, 1)
CEREAL_CLASS_VERSION(Trail::TrailConstants, 1)

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
void Trail::TrailParameter::serialize(Archive& archive, int version)
{
    archive
    (
        CEREAL_NVP(filename),
        CEREAL_NVP(rootpos),
        CEREAL_NVP(tailpos),
        CEREAL_NVP(lifetime),
        CEREAL_NVP(blend)
    );
}

template<class Archive>
void Trail::TrailConstants::serialize(Archive& archive, int version)
{
    archive
    (
        CEREAL_NVP(trailcolor),
        CEREAL_NVP(trailcolorscale),
        CEREAL_NVP(uvscroll)
    );
}

//コンストラクタ
Trail::Trail(const char* filename)
{
    Graphics& graphics = Graphics::Instance();
    ID3D11Device* device = graphics.GetDevice();

    // 入力レイアウト
    D3D11_INPUT_ELEMENT_DESC IED[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    CreateVsFromCso(device, "Shader\\TrailVS.cso", m_vertexshader.GetAddressOf(), m_inputlayout.ReleaseAndGetAddressOf(), IED, ARRAYSIZE(IED));

    // ピクセルシェーダー
    CreatePsFromCso(device, "Shader\\TrailPS.cso", m_pixelshader.GetAddressOf());

    // 頂点バッファ作成
    D3D11_BUFFER_DESC desc = {};
    desc.ByteWidth = sizeof(Vertex) * VertexCapacity;
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    HRESULT hr = device->CreateBuffer(&desc, nullptr, m_vertexbuffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

    //コンスタントバッファの作成
    D3D11_BUFFER_DESC buffer_desc{};
    buffer_desc.ByteWidth = sizeof(TrailConstants);
    buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    buffer_desc.CPUAccessFlags = 0;
    buffer_desc.MiscFlags = 0;
    buffer_desc.StructureByteStride = 0;
    hr = device->CreateBuffer(&buffer_desc, nullptr, m_constantbuffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

    //ファイル読み込み処理
    if (filename)
    {
        Desirialize(filename);
        filepath = filename;
        D3D11_TEXTURE2D_DESC texture2d_desc{};
        LoadTextureFromFile(device, TP.filename.c_str(), m_trailresource.GetAddressOf(), &texture2d_desc);
    }

    // オフセット値
    DirectX::XMVECTOR RootOffset = DirectX::XMVectorSet(0, 0, 0.0f, 0);
    DirectX::XMVECTOR TipOffset = DirectX::XMVectorSet(0, 0, 0.0f, 0);

    // 座標変換
    DirectX::XMFLOAT3 Rpos = TP.rootpos;
    DirectX::XMFLOAT3 Tpos = TP.tailpos;
    RootOffset = DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&Rpos), m_worldtransform);
    TipOffset = DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&Tpos), m_worldtransform);

    // trailpositionsに座標を保存
    XMStoreFloat3(&m_trailpositions[0][0], RootOffset);
    XMStoreFloat3(&m_trailpositions[1][0], TipOffset);
}

//更新処理
void Trail::Update(float elapsedTime)
{
    // 頂点バッファを1フレーム分ずらす
    for (int i = MAX_POLYGON - 1; i >= 1; --i)
    {
        m_trailpositions[0][i] = m_trailpositions[0][i - 1];
        m_trailpositions[1][i] = m_trailpositions[1][i - 1];
    }

    // オフセット値
    DirectX::XMVECTOR RootOffset = DirectX::XMVectorSet(0, 0, 0.0f, 0);
    DirectX::XMVECTOR TipOffset = DirectX::XMVectorSet(0, 0, 0.0f, 0);

    // 座標変換
    DirectX::XMFLOAT3 Rpos = GetGameObject()->transform_->GetWorldPosition() + TP.rootpos;
    DirectX::XMFLOAT3 Tpos = GetGameObject()->transform_->GetWorldPosition() + TP.tailpos;
    RootOffset = DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&Rpos), m_worldtransform);
    TipOffset = DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&Tpos), m_worldtransform);

    // trailpositionsに座標を保存
    XMStoreFloat3(&m_trailpositions[0][0], RootOffset);
    XMStoreFloat3(&m_trailpositions[1][0], TipOffset);

    // 変数定義
    const float CATMULL_ROM_STEP = 0.25f;
    float amount = 1.0f / (MAX_POLYGON - 1);
    float v = 0.0f;

    // スプライン曲線を実装する
    for (int i = 0; i < MAX_POLYGON; ++i)
    {
        int p1index = i;
        int p2index = min(i + 1, MAX_POLYGON - 1);
        int p3index = min(i + 2, MAX_POLYGON - 1);
        int p0index = max(i - 1, 0);

        // 上部の頂点
        DirectX::XMVECTOR upperpolygon0 = XMLoadFloat3(&m_trailpositions[0][p0index]);
        DirectX::XMVECTOR upperpolygon1 = XMLoadFloat3(&m_trailpositions[0][p1index]);
        DirectX::XMVECTOR upperpolygon2 = XMLoadFloat3(&m_trailpositions[0][p2index]);
        DirectX::XMVECTOR upperpolygon3 = XMLoadFloat3(&m_trailpositions[0][p3index]);

        // 下部の頂点
        DirectX::XMVECTOR lowerpolygon0 = XMLoadFloat3(&m_trailpositions[1][p0index]);
        DirectX::XMVECTOR lowerpolygon1 = XMLoadFloat3(&m_trailpositions[1][p1index]);
        DirectX::XMVECTOR lowerpolygon2 = XMLoadFloat3(&m_trailpositions[1][p2index]);
        DirectX::XMVECTOR lowerpolygon3 = XMLoadFloat3(&m_trailpositions[1][p3index]);

        v += amount;

        // スムーズなポリゴンを描画
        for (float j = 0; j <= 1.0f; j += CATMULL_ROM_STEP)
        {
            DirectX::XMVECTOR upperresult = DirectX::XMVectorCatmullRom(upperpolygon0, upperpolygon1, upperpolygon2, upperpolygon3, j);
            DirectX::XMVECTOR lowerresult = DirectX::XMVectorCatmullRom(lowerpolygon0, lowerpolygon1, lowerpolygon2, lowerpolygon3, j);
            DirectX::XMFLOAT3 upper, lower;
            XMStoreFloat3(&upper, upperresult);
            XMStoreFloat3(&lower, lowerresult);

            // テクスチャ座標を計算
            DirectX::XMFLOAT2 texUpper(1.0f, v); // 上部の頂点
            DirectX::XMFLOAT2 texLower(0.0f, v); // 下部の頂点

            // 計算された位置とテクスチャ座標を持つ頂点を追加
            AddVertex(upper, texUpper, TP.lifetime);
            AddVertex(lower, texLower, TP.lifetime);
        }
    }

    // 寿命時間の更新と削除
    for (auto it = m_vertices.begin(); it != m_vertices.end();)
    {
        it->lifetime -= elapsedTime;
        if (it->lifetime <= 0)
        {
            it = m_vertices.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

//描画
void Trail::Render()
{
    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();

    // シェーダーをセット
    dc->VSSetShader(m_vertexshader.Get(), nullptr, 0);
    dc->PSSetShader(m_pixelshader.Get(), nullptr, 0);

    // 入力レイアウトとプリミティブの設定
    dc->IASetInputLayout(m_inputlayout.Get());
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    // ブレンドステート、ラスタライザーステート、デプスステートの設定
    const float BlendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    dc->OMSetBlendState(graphics.GetBlendState(static_cast<BLENDSTATE>(TP.blend)), BlendFactor, 0xFFFFFFFF);
    dc->RSSetState(graphics.GetRasterizerState(RASTERIZERSTATE::SOLID_CULL_NONE));
    dc->OMSetDepthStencilState(graphics.GetDepthStencilState(DEPTHSTATE::ZT_ON_ZW_OFF), 0);

    //定数バッファの更新
    dc->UpdateSubresource(m_constantbuffer.Get(), 0, 0, &TC, 0, 0);
    dc->VSSetConstantBuffers(0, 1, m_constantbuffer.GetAddressOf());
    dc->PSSetConstantBuffers(0, 1, m_constantbuffer.GetAddressOf());

    // 頂点バッファをセット
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    dc->IASetVertexBuffers(0, 1, m_vertexbuffer.GetAddressOf(), &stride, &offset);

    // テクスチャをシェーダーにバインド
    dc->PSSetShaderResources(0, 1, m_trailresource.GetAddressOf());

    // 頂点データのバッファリングと描画
    UINT totalVertexCount = static_cast<UINT>(m_vertices.size());
    UINT start = 0;

    while (start < totalVertexCount)
    {
        UINT count = (std::min)(VertexCapacity, totalVertexCount - start);

        // 頂点バッファをマップしてデータをコピー
        D3D11_MAPPED_SUBRESOURCE mappedSubresource;
        HRESULT hr = dc->Map(m_vertexbuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
        _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

        memcpy(mappedSubresource.pData, &m_vertices[start], sizeof(Vertex) * count);

        dc->Unmap(m_vertexbuffer.Get(), 0);

        // 描画
        dc->Draw(count, 0);

        start += count;
    }

    // 頂点リストをクリア
    m_vertices.clear();

    // デプスステンシルの設定を元に戻す
    dc->OMSetDepthStencilState(graphics.GetDepthStencilState(DEPTHSTATE::ZT_OFF_ZW_ON), 0);

    // リソースの解放
    dc->IASetInputLayout(nullptr);
    dc->VSSetShader(nullptr, nullptr, 0);
    dc->PSSetShader(nullptr, nullptr, 0);
}

//gui
void Trail::OnGUI()
{
    if (ImGui::Button("Save"))
    {
        Serialize();
    }
    ImGui::SameLine();
    if (ImGui::Button("Load"))
    {
        LoadDesirialize();
        D3D11_TEXTURE2D_DESC texture2d_desc{};
        LoadTextureFromFile(Graphics::Instance().GetDevice(), TP.filename.c_str(), m_trailresource.GetAddressOf(), &texture2d_desc);
    }

    // ファイルパスを表示
    char filename[256];
    ::strncpy_s(filename, sizeof(filename), filepath.c_str(), sizeof(filename));
    if (ImGui::InputText((char*)u8"ファイルパス", filename, sizeof(filename), ImGuiInputTextFlags_EnterReturnsTrue))
    {
        filepath = filename;
    }

    //テクスチャロード
    char textureFile[256];
    ZeroMemory(textureFile, sizeof(textureFile));
    ::strncpy_s(textureFile, sizeof(textureFile), TP.filename.c_str(), sizeof(textureFile));
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
            TP.filename = relativeTextureFile;
            D3D11_TEXTURE2D_DESC texture2d_desc{};
            LoadTextureFromFile(Graphics::Instance().GetDevice(), TP.filename.c_str(), m_trailresource.GetAddressOf(), &texture2d_desc);
        }
    }
    ImGui::SameLine();
    ::strncpy_s(textureFile, sizeof(textureFile), TP.filename.c_str(), sizeof(textureFile));
    if (ImGui::InputText("texture", textureFile, sizeof(textureFile), ImGuiInputTextFlags_EnterReturnsTrue))
    {
        TP.filename = textureFile;

        char drive[32], dir[256], fullPath[256];
        ::_splitpath_s(textureFile, drive, sizeof(drive), dir, sizeof(dir), nullptr, 0, nullptr, 0);
        ::_makepath_s(fullPath, sizeof(fullPath), drive, dir, textureFile, nullptr);
        D3D11_TEXTURE2D_DESC texture2d_desc{};
        LoadTextureFromFile(Graphics::Instance().GetDevice(), TP.filename.c_str(), m_trailresource.GetAddressOf(), &texture2d_desc);
    }

    //デバッグ用にブレンドモード設定
    constexpr const char* BlendName[] =
    {
      "NONE",
      "ALPHA",
      "ADD",
      "SUBTRACT",
      "REPLACE",
      "MULTIPLY",
      "LIGHTEN",
      "DARKEN",
      "SCREEN",
      "MULTIPLERENDERTARGETS",
    };
    //ブレンドモード設定リストとのサイズが違うとエラーを出す
    static_assert(ARRAYSIZE(BlendName) != static_cast<int>(BLENDSTATE::MAX) - 1, "BlendName Size Error!");
    //ブレンドモード設定
    ImGui::Combo("BlendMode", &TP.blend, BlendName, static_cast<int>(BLENDSTATE::MAX), 10);

    ImGui::Text((char*)(u8"リソース"));
    ImGui::Image(m_trailresource.Get(), { 256, 256 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
    ImGui::DragFloat3((char*)u8"根本", &TP.rootpos.x, 0.1f);
    ImGui::DragFloat3((char*)u8"しっぽ", &TP.tailpos.x, 0.1f);
    ImGui::DragFloat((char*)u8"寿命時間", &TP.lifetime, 0.1f, 0.0f, 40.0f);
    ImGui::ColorEdit4((char*)u8"色", &TC.trailcolor.x);
    ImGui::DragFloat3((char*)u8"カラースケール", &TC.trailcolorscale.x, 0.1f, 0.0f, 300.0f);
    ImGui::DragFloat2((char*)u8"UVスクロール", &TC.uvscroll.x, 0.1f, 0.0f, 100.0f);
}

//トレイルに必用な行列と始点と終点のオフセット値を代入する関数
void Trail::SetTransform(const DirectX::XMFLOAT4X4& worldtransform, const DirectX::XMFLOAT3& tail, const DirectX::XMFLOAT3& root)
{
    m_worldtransform = DirectX::XMLoadFloat4x4(&worldtransform);
    TP.tailpos = tail;
    TP.rootpos = root;
}
void Trail::SetTransform(const DirectX::XMFLOAT4X4& worldtransform)
{
    m_worldtransform = DirectX::XMLoadFloat4x4(&worldtransform);
}

//シリアライズ
void Trail::Serialize()
{
    static const char* filter = "Trail Files(*.trail)\0*.trail;\0All Files(*.*)\0*.*;\0\0";

    char filename[256] = { 0 };
    DialogResult result = Dialog::SaveFileName(filename, sizeof(filename), filter, nullptr, "trail", Graphics::Instance().GetHwnd());
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
                    CEREAL_NVP(TP),
                    CEREAL_NVP(TC)
                );
            }
            catch (...)
            {
                LOG("trail deserialize failed.\n%s\n", filename);
                return;
            }
        }
    }
}

//デシリアライズ
void Trail::Desirialize(const char* filename)
{
    std::ifstream istream(filename, std::ios::binary);
    if (istream.is_open())
    {
        cereal::BinaryInputArchive archive(istream);

        try
        {
            archive
            (
                CEREAL_NVP(TP),
                CEREAL_NVP(TC)
            );
        }
        catch (...)
        {
            LOG("trail deserialize failed.\n%s\n", filename);
            return;
        }
    }
}

//読み込み
void Trail::LoadDesirialize()
{
    static const char* filter = "trail Files(*.trail)\0*.trail;\0All Files(*.*)\0*.*;\0\0";

    char filename[256] = { 0 };
    DialogResult result = Dialog::OpenFileName(filename, sizeof(filename), filter, nullptr, Graphics::Instance().GetHwnd());
    if (result == DialogResult::OK)
    {
        Desirialize(filename);
        filepath = filename;
    }
}

//頂点追加
void Trail::AddVertex(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT2& texcoord, float lifetime)
{
    Vertex& v = m_vertices.emplace_back();
    v.position = position;
    v.texcoord = texcoord;
    v.lifetime = lifetime;
}