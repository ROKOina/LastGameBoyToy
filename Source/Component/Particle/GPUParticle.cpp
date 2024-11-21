#include "GPUParticle.h"
#include "Graphics/Graphics.h"
#include "SystemStruct\Misc.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Math/Mathf.h"
#include "SystemStruct\Dialog.h"
#include "SystemStruct\Logger.h"
#include "Component/System/TransformCom.h"
#include <fstream>
#include <filesystem>
#include <shlwapi.h>
#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>

CEREAL_CLASS_VERSION(GPUParticle::SaveParameter, 2)
CEREAL_CLASS_VERSION(GPUParticle::GPUparticleSaveConstants, 2)

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
void GPUParticle::GPUparticleSaveConstants::serialize(Archive& archive, int version)
{
    archive
    (
        CEREAL_NVP(emitTime),
        CEREAL_NVP(lifeTime),
        CEREAL_NVP(stretchFlag),
        CEREAL_NVP(isLoopFlg),

        CEREAL_NVP(shape),

        CEREAL_NVP(baseColor),
        CEREAL_NVP(lifeStartColor),
        CEREAL_NVP(lifeEndColor),
        CEREAL_NVP(emitStartColor),
        CEREAL_NVP(emitEndColor),
        CEREAL_NVP(colorVariateByLife),
        CEREAL_NVP(colorScale),

        CEREAL_NVP(emitVec),
        CEREAL_NVP(spiralSpeed),
        CEREAL_NVP(orbitalVelocity),
        CEREAL_NVP(spiralstrong),
        CEREAL_NVP(veloRandScale),
        CEREAL_NVP(speed),
        CEREAL_NVP(emitStartSpeed),
        CEREAL_NVP(emitEndSpeed),

        CEREAL_NVP(scale),
        CEREAL_NVP(scaleVariateByLife),
        CEREAL_NVP(lifeStartSize),
        CEREAL_NVP(lifeEndSize),
        CEREAL_NVP(emitStartSize),
        CEREAL_NVP(emitEndSize),

        CEREAL_NVP(radial),
        CEREAL_NVP(buoyancy),
        CEREAL_NVP(emitStartGravity),
        CEREAL_NVP(emitEndGravity),

        CEREAL_NVP(strechscale),
        CEREAL_NVP(padding)
    );
    // バージョン1には存在しないフィールドにはデフォルト値を与える
    if (version == 1)
    {
        worldpos = 0;
    }
    if (version >= 2)
    {
        archive
        (
            CEREAL_NVP(worldpos)
        );
    }
}

template<class Archive>
void GPUParticle::SaveParameter::serialize(Archive& archive, int version)
{
    archive
    (
        CEREAL_NVP(m_blend),
        CEREAL_NVP(m_depthS),
        CEREAL_NVP(m_textureName)
    );
    // バージョン1には存在しないフィールドにはデフォルト値を与える
    if (version == 1)
    {
        m_deleteflag = false;
        deletetime = 0.0f;
    }
    if (version >= 2)
    {
        archive
        (
            CEREAL_NVP(m_deleteflag),
            CEREAL_NVP(deletetime)
        );
    }
}

//日本語にするマクロ
#define J(x) reinterpret_cast<const char*>(x)

UINT align(UINT num, UINT alignment)
{
    return (num + (alignment - 1)) & ~(alignment - 1);
}

//コンストラクタ
GPUParticle::GPUParticle(const char* filename, size_t maxparticle) :m_maxparticle(maxparticle)
{
    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();
    ID3D11Device* device = graphics.GetDevice();

    //パーティクルの構造体をバッファとして格納
    HRESULT hr{ S_OK };
    D3D11_BUFFER_DESC buffer_desc{};
    buffer_desc.ByteWidth = static_cast<UINT>(sizeof(EntityParticle) * maxparticle);
    buffer_desc.StructureByteStride = sizeof(EntityParticle);
    buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    buffer_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
    buffer_desc.CPUAccessFlags = 0;
    buffer_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    hr = device->CreateBuffer(&buffer_desc, NULL, m_particlebuffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

    //シェーダーリソースとして画像にして格納
    D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc{};
    shader_resource_view_desc.Format = DXGI_FORMAT_UNKNOWN;
    shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    shader_resource_view_desc.Buffer.ElementOffset = 0;
    shader_resource_view_desc.Buffer.NumElements = static_cast<UINT>(maxparticle);
    hr = device->CreateShaderResourceView(m_particlebuffer.Get(), &shader_resource_view_desc, m_particlesrv.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

    //ビュー作成
    D3D11_UNORDERED_ACCESS_VIEW_DESC unordered_access_view_desc{};
    unordered_access_view_desc.Format = DXGI_FORMAT_UNKNOWN;
    unordered_access_view_desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    unordered_access_view_desc.Buffer.FirstElement = 0;
    unordered_access_view_desc.Buffer.NumElements = static_cast<UINT>(maxparticle);
    unordered_access_view_desc.Buffer.Flags = 0;
    hr = device->CreateUnorderedAccessView(m_particlebuffer.Get(), &unordered_access_view_desc, m_particleuav.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

    //シェーダー読み込み
    CreateVsFromCso(device, "Shader\\GPUParticleVS.cso", m_vertexshader.ReleaseAndGetAddressOf(), NULL, NULL, 0);
    CreatePsFromCso(device, "Shader\\GPUParticlePS.cso", m_pixelshader.ReleaseAndGetAddressOf());
    CreateGsFromCso(device, "Shader\\GPUParticleGS.cso", m_geometryshader.ReleaseAndGetAddressOf());
    CreateCsFromCso(device, "Shader\\GPUParticleCS.cso", m_updatecomputeshader.ReleaseAndGetAddressOf());
    CreateCsFromCso(device, "Shader\\GPUParticleInitializeCS.cso", m_initialzecomputeshader.ReleaseAndGetAddressOf());

    //コンスタントバッファの定義と更新
    m_gpu = std::make_unique<ConstantBuffer<GPUParticleConstants>>(device);

    //コンスタントバッファのバッファ作成、更新
    buffer_desc.ByteWidth = sizeof(GPUparticleSaveConstants);
    buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    buffer_desc.CPUAccessFlags = 0;
    buffer_desc.MiscFlags = 0;
    buffer_desc.StructureByteStride = 0;
    hr = device->CreateBuffer(&buffer_desc, nullptr, m_constantbuffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

    //ファイル読み込み処理
    D3D11_TEXTURE2D_DESC texture2d_desc{};
    if (filename)
    {
        Desirialize(filename);
        filepath = filename;
        LoadTextureFromFile(Graphics::Instance().GetDevice(), m_p.m_textureName.c_str(), m_colormap.GetAddressOf(), &texture2d_desc);
    }
}

//初期設定
void GPUParticle::Start()
{
    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();

    //一時保存
    isFirstThrow = false;
    tempWorldPos = m_GSC.worldpos;
    m_GSC.worldpos = 0;

    m_gpu->data.position = (m_GSC.worldpos == 1) ? DirectX::XMFLOAT3{} : GetGameObject()->transform_->GetWorldPosition();
    m_gpu->data.rotation = GetGameObject()->transform_->GetRotation();
    m_gpu->data.world = GetGameObject()->transform_->GetWorldTransform();
    m_gpu->Activate(dc, (int)CB_INDEX::GPU_PARTICLE, false, false, true, true, false, false);
    dc->UpdateSubresource(m_constantbuffer.Get(), 0, 0, &m_GSC, 0, 0);
    dc->CSSetConstantBuffers((int)CB_INDEX::GPU_PARTICLE_SAVE, 1, m_constantbuffer.GetAddressOf());
    dc->GSSetConstantBuffers((int)CB_INDEX::GPU_PARTICLE_SAVE, 1, m_constantbuffer.GetAddressOf());
}

//更新処理
void GPUParticle::Update(float elapsedTime)
{
    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();

    //ここでループのONOFF制御を行っている無茶苦茶めんどくさい
    if (m_GSC.isLoopFlg)
    {
        if (!playedOnce) // isLoopFlg が true で、まだ再生されていない場合
        {
            activeflag = true;
            if (activeflag)
            {
                Play();
                activeflag = false;
                playedOnce = true; // 一度だけ再生したらフラグを更新
            }
        }
    }
    else
    {
        playedOnce = false; // isLoopFlg が false になったらリセット
    }

    // オブジェクトの回転に合わせてエフェクトも回転させる処理
    if (m_GSC.worldpos == 0)
    {
        DirectX::XMMATRIX transf = DirectX::XMLoadFloat4x4(&GetGameObject()->transform_->GetWorldTransform());
        DirectX::XMVECTOR velo = DirectX::XMLoadFloat3(&m_GSC.emitVec);
        velo = DirectX::XMVector3TransformNormal(velo, transf);
        DirectX::XMStoreFloat3(&m_gpu->data.currentEmitVec, velo);
    }

    // 単発再生
    if (m_GSC.isLoopFlg == false)
    {
        emitTimer += elapsedTime;

        if (emitTimer > m_GSC.emitTime)
        {
            m_gpu->data.isEmitFlg = false;
        }
    }

    //自身を消す関数
    if (m_p.m_deleteflag)
    {
        DeleteMe(elapsedTime);
    }

    //停止処理
    if (stopFlg == true)return;

    //コンスタントバッファの更新
    m_gpu->data.position = (m_GSC.worldpos == 1) ? DirectX::XMFLOAT3{} : GetGameObject()->transform_->GetWorldPosition();
    m_gpu->data.rotation = GetGameObject()->transform_->GetRotation();
    m_gpu->data.world = GetGameObject()->transform_->GetWorldTransform();
    m_gpu->Activate(dc, (int)CB_INDEX::GPU_PARTICLE, false, false, true, true, false, false);
    dc->UpdateSubresource(m_constantbuffer.Get(), 0, 0, &m_GSC, 0, 0);
    dc->CSSetConstantBuffers((int)CB_INDEX::GPU_PARTICLE_SAVE, 1, m_constantbuffer.GetAddressOf());
    dc->GSSetConstantBuffers((int)CB_INDEX::GPU_PARTICLE_SAVE, 1, m_constantbuffer.GetAddressOf());

    //更新するコンピュートシェーダーをセットする
    dc->CSSetUnorderedAccessViews(0, 1, m_particleuav.GetAddressOf(), NULL);
    dc->CSSetShader(m_updatecomputeshader.Get(), NULL, 0);
    const UINT thread_group_count_x = align(static_cast<UINT>(m_maxparticle), THREAD) / THREAD;
    dc->Dispatch(thread_group_count_x, 1, 1);
    ID3D11UnorderedAccessView* null_unordered_access_view{};
    dc->CSSetUnorderedAccessViews(0, 1, &null_unordered_access_view, NULL);
}

//描画
void GPUParticle::Render()
{
    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();

    //ブレンドステート設定
    const float BlendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    dc->OMSetBlendState(graphics.GetBlendState(static_cast<BLENDSTATE>(m_p.m_blend)), BlendFactor, 0xFFFFFFFF);

    //ラスタライザの設定
    dc->RSSetState(graphics.GetRasterizerState(RASTERIZERSTATE::SOLID_CULL_NONE));

    //画像を渡す
    dc->PSSetShaderResources(20, 1, m_colormap.GetAddressOf());

    //デプスステンシルステート設定
    dc->OMSetDepthStencilState(graphics.GetDepthStencilState(static_cast<DEPTHSTATE>(m_p.m_depthS)), 0);

    //プリミティブトポロジー設定
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

    //シェーダーセット
    dc->VSSetShader(m_vertexshader.Get(), NULL, 0);
    dc->PSSetShader(m_pixelshader.Get(), NULL, 0);
    dc->GSSetShader(m_geometryshader.Get(), NULL, 0);
    dc->GSSetShaderResources(0, 1, m_particlesrv.GetAddressOf());

    //コンスタントバッファの更新
    m_gpu->data.position = (m_GSC.worldpos == 1) ? DirectX::XMFLOAT3{} : GetGameObject()->transform_->GetWorldPosition();
    m_gpu->data.rotation = GetGameObject()->transform_->GetRotation();
    m_gpu->data.world = GetGameObject()->transform_->GetWorldTransform();
    m_gpu->Activate(dc, (int)CB_INDEX::GPU_PARTICLE, false, false, true, true, false, false);
    dc->UpdateSubresource(m_constantbuffer.Get(), 0, 0, &m_GSC, 0, 0);
    dc->CSSetConstantBuffers((int)CB_INDEX::GPU_PARTICLE_SAVE, 1, m_constantbuffer.GetAddressOf());
    dc->GSSetConstantBuffers((int)CB_INDEX::GPU_PARTICLE_SAVE, 1, m_constantbuffer.GetAddressOf());

    //解放
    dc->IASetInputLayout(NULL);
    dc->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
    dc->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, 0);
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
    dc->Draw(static_cast<UINT>(m_maxparticle), 0);

    //デプスステンシルステート設定
    dc->OMSetDepthStencilState(graphics.GetDepthStencilState(DEPTHSTATE::ZT_OFF_ZW_ON), 0);

    //解放
    ID3D11ShaderResourceView* null_shader_resource_view{};
    dc->GSSetShaderResources(0, 1, &null_shader_resource_view);
    dc->VSSetShader(NULL, NULL, 0);
    dc->PSSetShader(NULL, NULL, 0);
    dc->GSSetShader(NULL, NULL, 0);
    dc->CSSetShader(NULL, NULL, 0);

    //一度だけ通る
    if (!isFirstThrow)
    {
        m_GSC.worldpos = tempWorldPos;  //保存を適用
        isFirstThrow = true;
    }
}

//imgui
void GPUParticle::OnGUI()
{
    SystemGUI();

    //テクスチャロード
    char textureFile[256];
    ZeroMemory(textureFile, sizeof(textureFile));
    ::strncpy_s(textureFile, sizeof(textureFile), m_p.m_textureName.c_str(), sizeof(textureFile));
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
            m_p.m_textureName = relativeTextureFile;
            D3D11_TEXTURE2D_DESC texture2d_desc{};
            LoadTextureFromFile(Graphics::Instance().GetDevice(), m_p.m_textureName.c_str(), m_colormap.GetAddressOf(), &texture2d_desc);
        }
    }
    ImGui::SameLine();
    ::strncpy_s(textureFile, sizeof(textureFile), m_p.m_textureName.c_str(), sizeof(textureFile));
    if (ImGui::InputText("texture", textureFile, sizeof(textureFile), ImGuiInputTextFlags_EnterReturnsTrue))
    {
        m_p.m_textureName = textureFile;

        char drive[32], dir[256], fullPath[256];
        ::_splitpath_s(textureFile, drive, sizeof(drive), dir, sizeof(dir), nullptr, 0, nullptr, 0);
        ::_makepath_s(fullPath, sizeof(fullPath), drive, dir, textureFile, nullptr);
        D3D11_TEXTURE2D_DESC texture2d_desc{};
        LoadTextureFromFile(Graphics::Instance().GetDevice(), m_p.m_textureName.c_str(), m_colormap.GetAddressOf(), &texture2d_desc);
    }
    ImGui::Text(J(u8"リソース"));
    ImGui::Image(m_colormap.Get(), { 256, 256 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });

    ParameterGUI();
}

void GPUParticle::SystemGUI()
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
        LoadTextureFromFile(Graphics::Instance().GetDevice(), m_p.m_textureName.c_str(), m_colormap.GetAddressOf(), &texture2d_desc);
    }
    ImGui::SameLine();
    if (ImGui::Button("PLAY"))
    {
        //リセット関数
        Play();
    }
    ImGui::SameLine();
    if (ImGui::Button("ParameterReset"))
    {
        //パラメータリセット関数
        ParameterReset();
    }

    // ファイルパスを表示
    char filename[256];
    ::strncpy_s(filename, sizeof(filename), filepath.c_str(), sizeof(filename));
    if (ImGui::InputText((char*)u8"ファイルパス", filename, sizeof(filename), ImGuiInputTextFlags_EnterReturnsTrue))
    {
        filepath = filename;
    }

    ImGui::Checkbox(J(u8"生存フラグ"), reinterpret_cast<bool*>(&m_gpu->data.isalive));
    ImGui::SameLine();
    ImGui::Checkbox(J(u8"ループ"), reinterpret_cast<bool*>(&m_GSC.isLoopFlg));
    ImGui::SameLine();
    ImGui::Checkbox(J(u8"停止"), &stopFlg);
    ImGui::Checkbox(J(u8"ストレッチビルボード"), reinterpret_cast<bool*>(&m_GSC.stretchFlag));
    ImGui::SameLine();
    ImGui::Checkbox(J(u8"パーティクル遅延なし"), reinterpret_cast<bool*>(&m_GSC.worldpos));
    if (m_GSC.stretchFlag == 1)
    {
        ImGui::DragFloat(J(u8"ストレッチビルボードの伸ばす係数"), &m_GSC.strechscale, 0.1f, 1.0f, 100.0f);
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
    ImGui::Combo("BlendMode", &m_p.m_blend, BlendName, static_cast<int>(BLENDSTATE::MAX), 10);

    //デバッグ用に深度ステート設定
    constexpr const char* dsName[] =
    {
      "NONE",
      "ZT_ON_ZW_ON",
      "ZT_ON_ZW_OFF",
      "ZT_OFF_ZW_ON",
      "ZT_OFF_ZW_OFF",
      "SILHOUETTE",
      "MASK",
      "APPLY_MASK",
      "EXCLUSIVE",
    };
    //ブレンドモード設定リストとのサイズが違うとエラーを出す
    static_assert(ARRAYSIZE(dsName) != static_cast<int>(DEPTHSTATE::MAX) - 1, "DEPTHSTATE Size Error!");
    //ブレンドモード設定
    ImGui::Combo("DepthState", &m_p.m_depthS, dsName, static_cast<int>(DEPTHSTATE::MAX), ARRAYSIZE(dsName));
}

void GPUParticle::ParameterGUI()
{
    if (ImGui::TreeNode(J(u8"パーティクルの生成削除関係")))
    {
        ImGui::Checkbox(J(u8"削除フラグ"), &m_p.m_deleteflag);
        ImGui::DragFloat(J(u8"削除時間"), &m_p.deletetime, 0.1f, 0.0f, 10.0f);
        ImGui::DragFloat(J(u8"エフェクトの再生時間"), &m_GSC.emitTime, 0.1f);
        ImGui::DragFloat(J(u8"パーティクルの寿命"), &m_GSC.lifeTime, 0.1f, 0.0f, 5.0f);
        ImGui::TreePop();
    }
    EmitGUI();
    SpeedGUI();
    ScaleGUI();
    ColorGUI();
}

void GPUParticle::ColorGUI()
{
    if (ImGui::TreeNode(J(u8"カラー関係"))) {
        ImGui::ColorEdit4(J(u8"ベースの色"), &m_GSC.baseColor.x);
        ImGui::DragFloat3(J(u8"カラースケール"), &m_GSC.colorScale.x, 0.1f);

        ImGui::Checkbox(J(u8"パーティクルの寿命によって変化"), (bool*)&m_GSC.colorVariateByLife);

        if (m_GSC.scaleVariateByLife) {
            ImGui::ColorEdit4(J(u8"始めの色 "), &m_GSC.lifeStartColor.x);
            ImGui::ColorEdit4(J(u8"最後の色 "), &m_GSC.lifeEndColor.x);
        }
        else {
            ImGui::ColorEdit4(J(u8"始めの色"), &m_GSC.emitStartColor.x);
            ImGui::ColorEdit4(J(u8"最後の色"), &m_GSC.emitEndColor.x);
        }

        ImGui::TreePop();
    }
}

void GPUParticle::ScaleGUI()
{
    if (ImGui::TreeNode(J(u8"スケール関係"))) {
        ImGui::DragFloat2(J(u8"スケールXY"), &m_GSC.scale.x, 0.01f, 0);
        if (ImGui::DragFloat(J(u8"スケール"), &m_GSC.scale.x, 0.01f, 0))
        {
            m_GSC.scale.y = m_GSC.scale.x;
        }

        ImGui::Checkbox(J(u8"パーティクルの寿命によって変化 "), (bool*)&m_GSC.scaleVariateByLife);

        if (m_GSC.scaleVariateByLife) {
            ImGui::SetNextItemWidth(90);
            ImGui::DragFloat(J(u8"最初の大きさ "), &m_GSC.lifeStartSize, 0.01f, 0.01f, 1.0f);
            ImGui::SameLine();
            ImGui::SetNextItemWidth(90);
            ImGui::DragFloat(J(u8"最後の大きさ "), &m_GSC.lifeEndSize, 0.01f, 0.01f, 1.0f);
        }
        else {
            ImGui::SetNextItemWidth(90);
            ImGui::DragFloat(J(u8"最初の大きさ"), &m_GSC.emitStartSize, 0.01f, 0.01f, 1.0f);
            ImGui::SameLine();
            ImGui::SetNextItemWidth(90);
            ImGui::DragFloat(J(u8"最後の大きさ"), &m_GSC.emitEndSize, 0.01f, 0.01f, 1.0f);
        }

        ImGui::TreePop();
    }
}

void GPUParticle::SpeedGUI()
{
    if (ImGui::TreeNode(J(u8"スピード関係"))) {
        ImGui::DragFloat(J(u8"スピード"), &m_GSC.speed, 0.1f, 0.0f, 100.0f);
        ImGui::SetNextItemWidth(90);
        ImGui::DragFloat(J(u8"最初のスピード"), &m_GSC.emitStartSpeed, 0.1f, 0.01f, 100.0f);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(90);
        ImGui::DragFloat(J(u8"最後のスピード"), &m_GSC.emitEndSpeed, 0.1f, 0.01f, 100.0f);

        ImGui::SetNextItemWidth(90);
        ImGui::DragFloat(J(u8"速力ランダム"), &m_GSC.veloRandScale, 0.1f, 0.0f, 100.0f);
        ImGui::DragFloat(J(u8"浮力"), &m_GSC.buoyancy, 0.1f, -100.0f, 100.0f);
        ImGui::SetNextItemWidth(90);
        ImGui::DragFloat(J(u8"最初の重力"), &m_GSC.emitStartGravity, 0.01f, 0.0f, 100.0f);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(90);
        ImGui::DragFloat(J(u8"最後の重力"), &m_GSC.emitEndGravity, 0.01f, 0.0f, 100.0f);

        ImGui::TreePop();
    }
}

void GPUParticle::EmitGUI()
{
    if (ImGui::TreeNode(J(u8"パーティクルの飛ばし方 関係"))) {
        if (ImGui::DragFloat3(J(u8"放射ベクトル"), &m_GSC.emitVec.x, 0.1f)) {
            m_GSC.emitVec = Mathf::Normalize(m_GSC.emitVec);
        }

        ImGui::DragFloat3(J(u8"回転係数"), &m_GSC.orbitalVelocity.x, 0.1f);
        ImGui::SetNextItemWidth(90);
        ImGui::DragFloat(J(u8"生成場所ランダム"), &m_GSC.shape.x, 0.1f, 0.0f, 100.0f);

        ImGui::SetNextItemWidth(90);
        ImGui::DragFloat(J(u8"生成角度"), &m_GSC.shape.w, 0.1f, 0.0f, 1.0f);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(90);
        ImGui::DragFloat(J(u8"球の大きさ"), &m_GSC.shape.y, 0.1f, 0.0f, 360.0f);
        ImGui::SetNextItemWidth(90);
        ImGui::DragFloat(J(u8"球の集まり具合"), &m_GSC.shape.z, 0.1f, 0.0f, 1.0f);
        ImGui::SetNextItemWidth(90);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(90);
        ImGui::DragFloat(J(u8"弾け飛ぶ係数"), &m_GSC.radial, 0.1f, -10.0f, 10.0f);
        ImGui::SetNextItemWidth(90);
        ImGui::DragFloat(J(u8"スパイラル速度"), &m_GSC.spiralSpeed, 0.1f, 0.0f, 20.0f);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(90);
        ImGui::DragFloat(J(u8"スパイラル強度"), &m_GSC.spiralstrong, 0.1f, 0.0f, 20.0f);

        ImGui::TreePop();
    }
}

//リセット関数
void GPUParticle::Play()
{
    m_gpu->data.isEmitFlg = true;
    emitTimer = 0.0f;

    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();

    //コンスタントバッファの更新
    m_gpu->data.position = (m_GSC.worldpos == 1) ? DirectX::XMFLOAT3{} : GetGameObject()->transform_->GetWorldPosition();
    m_gpu->data.rotation = GetGameObject()->transform_->GetRotation();
    m_gpu->data.world = GetGameObject()->transform_->GetWorldTransform();
    m_gpu->Activate(dc, (int)CB_INDEX::GPU_PARTICLE, false, false, true, true, false, false);
    dc->UpdateSubresource(m_constantbuffer.Get(), 0, 0, &m_GSC, 0, 0);
    dc->CSSetConstantBuffers((int)CB_INDEX::GPU_PARTICLE_SAVE, 1, m_constantbuffer.GetAddressOf());
    dc->GSSetConstantBuffers((int)CB_INDEX::GPU_PARTICLE_SAVE, 1, m_constantbuffer.GetAddressOf());

    //初期化のピクセルシェーダーをセット
    dc->CSSetUnorderedAccessViews(0, 1, m_particleuav.GetAddressOf(), NULL);
    dc->CSSetShader(m_initialzecomputeshader.Get(), NULL, 0);
    const UINT thread_group_count_x = align(static_cast<UINT>(m_maxparticle), THREAD) / THREAD;
    dc->Dispatch(thread_group_count_x, 1, 1);
    ID3D11UnorderedAccessView* null_unordered_access_view{};
    dc->CSSetUnorderedAccessViews(0, 1, &null_unordered_access_view, NULL);
}

//自身を消す関数
void GPUParticle::DeleteMe(float elapsedTime)
{
    time += elapsedTime;
    if (time > m_p.deletetime)
    {
        GameObjectManager::Instance().Remove(GetGameObject());
    }
}

//シリアライズ
void GPUParticle::Serialize()
{
    static const char* filter = "Particle Files(*.gpuparticle)\0*.gpuparticle;\0All Files(*.*)\0*.*;\0\0";

    char filename[256] = { 0 };
    DialogResult result = Dialog::SaveFileName(filename, sizeof(filename), filter, nullptr, "gpuparticle", Graphics::Instance().GetHwnd());
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
                    CEREAL_NVP(m_GSC),
                    CEREAL_NVP(m_p)
                );
            }
            catch (...)
            {
                LOG("particle deserialize failed.\n%s\n", filename);
                return;
            }
        }
    }
}

//デシリアライズ
void GPUParticle::Desirialize(const char* filename)
{
    std::ifstream istream(filename, std::ios::binary);
    if (istream.is_open())
    {
        cereal::BinaryInputArchive archive(istream);

        try
        {
            archive
            (
                CEREAL_NVP(m_GSC),
                CEREAL_NVP(m_p)
            );
        }
        catch (...)
        {
            LOG("particle deserialize failed.\n%s\n", filename);
            return;
        }
    }
}

//読み込み
void GPUParticle::LoadDesirialize()
{
    static const char* filter = "particle Files(*.gpuparticle)\0*.gpuparticle;\0All Files(*.*)\0*.*;\0\0";

    char filename[256] = { 0 };
    DialogResult result = Dialog::OpenFileName(filename, sizeof(filename), filter, nullptr, Graphics::Instance().GetHwnd());
    if (result == DialogResult::OK)
    {
        Desirialize(filename);
        filepath = filename;
    }
}

//パラメータリセット
void GPUParticle::ParameterReset()
{
    m_GSC.lifeTime = 1.0f;
    m_GSC.shape = { 0.0f,0.0f,0.0f,0.0f };

    m_GSC.baseColor = { 1,1,1,1 };
    m_GSC.lifeStartColor = { 1,1,1,1 };
    m_GSC.lifeEndColor = { 1,1,1,1 };
    m_GSC.emitStartColor = { 1,1,1,1 };
    m_GSC.emitEndColor = { 1,1,1,1 };
    m_GSC.colorScale = { 1,1,1 };

    m_GSC.emitVec = { 0,0,0 };
    m_GSC.spiralSpeed = { 0 };
    m_GSC.orbitalVelocity = { 0,0,0 };
    m_GSC.veloRandScale = 0.0f;
    m_GSC.speed = 1.0f;
    m_GSC.emitStartSpeed = 1.0f;
    m_GSC.emitEndSpeed = 1.0f;

    m_GSC.scale = { 0.2f,0.2f };
    m_GSC.lifeStartSize = 1.0f;
    m_GSC.lifeEndSize = 1.0f;
    m_GSC.emitStartSize = 1.0f;
    m_GSC.emitEndSize = 1.0f;

    m_GSC.radial = { 0 };
    m_GSC.buoyancy = {};
    m_GSC.emitStartGravity = 0.0f;
    m_GSC.emitEndGravity = 0.0f;
    m_GSC.strechscale = 1.0f;
}