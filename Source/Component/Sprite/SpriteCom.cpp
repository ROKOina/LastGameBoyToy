#include "SpriteCom.h"
#include "SystemStruct\Misc.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Math/easing.h"

SpriteCom::SpriteCom(const char* filename, SpriteShader spriteshader)
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


    //ファイル名
    const char* PSPath = nullptr;
    const char* VSPath = nullptr;
    //増やしたいシェーダーがあればスイッチ文で複製していく
    switch (spriteshader)
    {
    case SpriteShader::DEFALT:
        PSPath = { "Shader\\SpritePS.cso" };
        VSPath = { "Shader\\SpriteVS.cso" };
        break;
    case SpriteShader::DEFALTUV:
        PSPath = { "Shader\\SpriteDefaltUVPS.cso" };
        VSPath = { "Shader\\SpriteVS.cso" };
        break;
    case SpriteShader::BLUR:
        PSPath = { "Shader\\SpriteBlurPS.cso" };
        VSPath = { "Shader\\SpriteVS.cso" };
        break;
    case SpriteShader::CHROMATICABERRATION:
        PSPath = { "Shader\\SpriteChromaticAberrationPS.cso" };
        VSPath = { "Shader\\SpriteVS.cso" };
        break;
    case SpriteShader::DISSOLVE:
        PSPath = { "Shader\\SpriteDissolvePS.cso" };
        VSPath = { "Shader\\SpriteVS.cso" };
        break;
    case SpriteShader::GLITCH:
        PSPath = { "Shader\\GlitchPS.cso" };
        VSPath = { "Shader\\SpriteVS.cso" };
        break;
    case SpriteShader::HOLO:
        PSPath = { "Shader\\HoloGlennPS.cso" };
        VSPath = { "Shader\\SpriteVS.cso" };
        break;
    case SpriteShader::ELECTRO:
        PSPath = { "Shader\\SpritePS.cso" };
        VSPath = { "Shader\\ElectroVS.cso" };
        break;
    default:
        assert(!"シェーダーがありません");
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
        CreateVsFromCso(device, VSPath, vertexShader_.GetAddressOf(), inputLayout_.GetAddressOf(), inputElementDesc, _countof(inputElementDesc));
    }

    // ピクセルシェーダー
    {
        CreatePsFromCso(device, PSPath, pixelShader_.GetAddressOf());
    }

    //ファイル読み込み処理
    if (filename != "")
    {
        this->filename = filename;
        Deserialize(filename);
        LoadTextureFromFile(device, spc.filename.c_str(), shaderResourceView_.GetAddressOf(), &texture2ddesc_);
    }
}

void SpriteCom::Render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
    Graphics& Graphics = Graphics::Instance();
    ID3D11Device* device = Graphics.GetDevice();
    ID3D11DeviceContext* dc = Graphics.GetDeviceContext();

    DrawStart();

    // デプスステンシルをクリアする
    if (spc.clearDepthStencilEnable)dc->ClearDepthStencilView(Graphics::Instance().GetDepthStencilView(), D3D11_CLEAR_STENCIL, 1, 0);

    // ステートの設定
    dc->OMSetBlendState(Graphics.GetBlendState(static_cast<BLENDSTATE>(spc.blend)), nullptr, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(Graphics.GetDepthStencilState(static_cast<DEPTHSTATE>(spc.depth)), 1);
    dc->RSSetState(Graphics.GetRasterizerState(RASTERIZERSTATE::SOLID_CULL_NONE));

    Draw(view, projection);

    if (spc.showMaskGraphicEnable)
    {
        dc->OMSetDepthStencilState(Graphics.GetDepthStencilState(DEPTHSTATE::NONE), 1);
        Draw(view, projection);
    }

    DrawEnd();
}

void SpriteCom::DrawStart()
{
    for (auto& start : drawStart)
    {
        if (start != nullptr)
        {
            // 登録された関数を実行
            start();
        }
    }
}

void SpriteCom::Draw(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
    Graphics& Graphics = Graphics::Instance();
    ID3D11Device* device = Graphics.GetDevice();
    ID3D11DeviceContext* dc = Graphics.GetDeviceContext();

    // ビューポート設定
    D3D11_VIEWPORT viewport{};
    UINT num_viewports{ 1 };
    dc->RSGetViewports(&num_viewports, &viewport);

    // 変換行列
    DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&view);
    DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&projection);

    Vector2        position = { GetGameObject()->transform_->GetWorldPosition().x, GetGameObject()->transform_->GetWorldPosition().y };
    Vector2        scale = { GetGameObject()->transform_->GetScale().x   , GetGameObject()->transform_->GetScale().y };
    float          angle = GetGameObject()->transform_->GetEulerRotation().z;
    const Vector2& texSize = spc.texSize;
    const Vector2& texPos = spc.texPos;
    const Vector2& pivot = spc.pivot;

    // ピボットの処理
    Vector2 drawPivot = ((texSize * scale) / texSize) * pivot;

    position -= drawPivot;

    // 描画範囲
    Vector2 extent = texSize * scale;

    // アフィン変換
    // スプライトを構成する４頂点のスクリーン座標を計算する
    Vector2 positions[] = {
        Vector2(position.x            , position.y),			    // 左上
        Vector2(position.x + extent.x , position.y),			    // 右上
        Vector2(position.x            , position.y + extent.y),	// 左下
        Vector2(position.x + extent.x , position.y + extent.y),	// 右下
    };

    // スプライトを構成する４頂点のテクスチャ座標を計算する
    Vector2 texcoords[] = {
        Vector2(texPos.x            , texPos.y),			        // 左上
        Vector2(texPos.x + texSize.x, texPos.y),			        // 右上
        Vector2(texPos.x            , texPos.y + texSize.y),	    // 左下
        Vector2(texPos.x + texSize.x, texPos.y + texSize.y),	    // 右下
    };

    // スプライトの中心で回転させるために４頂点の中心位置が
    // 原点(0, 0)になるように一旦頂点を移動させる。
    Vector2 offset = position + extent * (pivot / texSize);
    for (auto& p : positions)
    {
        p -= offset;
    }

    // 頂点を回転させる
    float theta = angle /** (DirectX::XM_PI / 180.0f)*/;	// 角度をラジアン(θ)に変換
    float c = cosf(theta);
    float s = sinf(theta);
    for (auto& p : positions)
    {
        DirectX::XMFLOAT2 r = p;
        p.x = c * r.x + -s * r.y;
        p.y = s * r.x + c * r.y;
    }

    // 回転のために移動させた頂点を元の位置に戻す
    for (auto& p : positions)
    {
        p += offset;
    }

    // スクリーン座標系からNDC座標系へ変換する。
    for (auto& p : positions)
    {
        p.x = 2.0f * p.x / viewport.Width - 1.0f;
        p.y = 1.0f - 2.0f * p.y / viewport.Height;
    }

    float texWidth = (float)texture2ddesc_.Width;
    float texHeight = (float)texture2ddesc_.Height;

    // 頂点バッファのデータを作成
    for (int i = 0; i < 4; ++i)
    {
        texcoords[i].x /= texWidth;
        texcoords[i].y /= texHeight;
    }

    // 頂点バッファのマッピング
    D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
    HRESULT hr = dc->Map(vertexBuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

    Vertex* vertices = reinterpret_cast<Vertex*>(mapped_subresource.pData);
    if (vertices != nullptr)
    {
        for (int i = 0; i < 4; i++)
        {
            vertices[i].position = DirectX::XMFLOAT3(positions[i].x, positions[i].y, 0);
            vertices[i].texcoord = texcoords[i];
        }

        vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = { spc.color.x, spc.color.y, spc.color.z, spc.color.w };
    }
    dc->Unmap(vertexBuffer_.Get(), 0);

    // 描画設定
    UINT stride = sizeof(Vertex);
    UINT pOffset = 0;
    dc->IASetVertexBuffers(0, 1, vertexBuffer_.GetAddressOf(), &stride, &pOffset);
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    dc->IASetInputLayout(inputLayout_.Get());
    dc->VSSetShader(vertexShader_.Get(), nullptr, 0);
    dc->PSSetShader(pixelShader_.Get(), nullptr, 0);
    dc->PSSetShaderResources(0, 1, shaderResourceView_.GetAddressOf());
    dc->Draw(4, 0);
}

void SpriteCom::DrawEnd()
{
    for (auto& end : drawEnd)
    {
        if (end != nullptr)
        {
            // 登録された関数を実行
            end();
        }
    }
}

void SpriteCom::OnGUI()
{
#ifdef DEBUG
    if (ImGui::IsKeyDown(ImGuiKey_LeftShift))
    {
        const Mouse& mouse = Input::Instance().GetMouse();
        Vector2 mousePos = { (float)(mouse.GetPositionX()) ,(float)(mouse.GetPositionY()) };
        if (Mouse::BTN_LEFT & mouse.GetButtonDown())
        {
            Vector2 rectPos = { GetGameObject()->transform_->GetWorldPosition().x,GetGameObject()->transform_->GetWorldPosition().y };
            offsetPos = (rectPos - mousePos);
        }

        if (Mouse::BTN_LEFT & mouse.GetButton())
        {
            Vector2 setPos = (offsetPos + mousePos);
            GetGameObject()->transform_->SetWorldPosition({ setPos.x, setPos.y, 0 });
        }
    }
#endif // DEBUG

    if (ImGui::Button((char*)u8"保存"))
    {
        Serialize();
    }
    if (filename != "")
    {
        ImGui::SameLine();
        COPY_GUI(filename)
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
            LoadTexture(relativeTextureFile);
            spc.pivot = { texture2ddesc_.Width / 2.0f,texture2ddesc_.Height / 2.0f };
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

    ImGui::Separator();
    if (ImGui::Button("Right Up      "))
    {
        spc.pivot = { 0,0 };
    }
    ImGui::SameLine();
    if (ImGui::Button("Center Up     "))
    {
        
        spc.pivot = { static_cast<float>(texture2ddesc_.Width) * 0.5f,0 };
    }
    ImGui::SameLine();
    if (ImGui::Button("Left Up      "))
    {
        spc.pivot = { static_cast<float>(texture2ddesc_.Width) ,0 };
    }
    if (ImGui::Button("Right Center"))
    {
        spc.pivot = { 0,static_cast<float>(texture2ddesc_.Height) * 0.5f };
    }
    ImGui::SameLine();
    if (ImGui::Button("Center          "))
    {
        spc.pivot = { static_cast<float>(texture2ddesc_.Width) * 0.5f,static_cast<float>(texture2ddesc_.Height) * 0.5f };
    }
    ImGui::SameLine();
    if (ImGui::Button("Left Center"))
    {
        spc.pivot = { static_cast<float>(texture2ddesc_.Width) ,static_cast<float>(texture2ddesc_.Height) * 0.5f };
    }
    if (ImGui::Button("Right Down  "))
    {
        spc.pivot = { 0,static_cast<float>(texture2ddesc_.Height) };
    }
    ImGui::SameLine();
    if (ImGui::Button("Center Down"))
    {
        spc.pivot = { static_cast<float>(texture2ddesc_.Width) * 0.5f,static_cast<float>(texture2ddesc_.Height) };
    }
    ImGui::SameLine();
    if (ImGui::Button("Left Down "))
    {
        spc.pivot = { static_cast<float>(texture2ddesc_.Width) ,static_cast<float>(texture2ddesc_.Height) };
    }
    ImGui::DragFloat2("Pivot", &spc.pivot.x);
    ImGui::Separator();
    
    ImGui::DragFloat2("TexSize", &spc.texSize.x);

    ImGui::DragFloat2("TexPos", &spc.texPos.x);
    
    ImGui::ColorEdit4("Sprite Color", &spc.color.x);

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

}

void SpriteCom::LoadTexture(std::string filename)
{
    spc.filename = filename;
    shaderResourceView_ = nullptr;
    LoadTextureFromFile(Graphics::Instance().GetDevice(), spc.filename.c_str(), shaderResourceView_.GetAddressOf(), &texture2ddesc_);
    spc.texSize.x = texture2ddesc_.Width;
    spc.texSize.y = texture2ddesc_.Height;
}

void SpriteCom::Serialize()
{
    static const char* filter = "spc Files(*.spc)\0*.spc;\0All Files(*.*)\0*.*;\0\0";

    char filename[256] = { 0 };
    DialogResult result = Dialog::SaveFileName(filename, sizeof(filename), filter, nullptr, "spc", Graphics::Instance().GetHwnd());
    if (result == DialogResult::OK)
    {
        this->filename = filename;
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
                LOG("spc deserialize failed.\n%s\n", filename);
                return;
            }
        }
    }
}

void SpriteCom::Deserialize(const char* filename)
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
            LOG("spc deserialize failed.\n%s\n", filename);
            return;
        }
    }
}

// シリアライズ
template<class Archive>
void SpriteCom::SaveParameterCPU::serialize(Archive& archive, int version)
{
    if (version == 0)
    {
        archive
        (
            CEREAL_NVP(color),
            CEREAL_NVP(filename),
            CEREAL_NVP(pivot),
            CEREAL_NVP(texPos),
            CEREAL_NVP(texSize),
            CEREAL_NVP(color),
            CEREAL_NVP(orderinLayer),
            CEREAL_NVP(blend),
            CEREAL_NVP(depth),
            CEREAL_NVP(clearDepthStencilEnable),
            CEREAL_NVP(showMaskGraphicEnable)
        );
    }
}
CEREAL_CLASS_VERSION(SpriteCom::SaveParameterCPU, 0)