#include "CPUParticle.h"
#include "Graphics/Graphics.h"
#include "Graphics/Shaders/Shader.h"
#include "Dialog.h"
#include "GameSource/Math/Mathf.h"
#include "Logger.h"
#include "Components/TransformCom.h"
#include "GameSource\Math\Mathf.h"
#include <fstream>
#include <filesystem>
#include <shlwapi.h>
#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>

CEREAL_CLASS_VERSION(CPUParticle::SerializeCPUParticle, 1)

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
void CPUParticle::SerializeCPUParticle::serialize(Archive& archive, int version)
{
    archive
    (
        CEREAL_NVP(m_verticalkoma),
        CEREAL_NVP(m_besidekoma),
        CEREAL_NVP(m_animationspeed),
        CEREAL_NVP(m_max),
        CEREAL_NVP(m_lifetime),
        CEREAL_NVP(m_radius),
        CEREAL_NVP(m_scaleradius),
        CEREAL_NVP(m_arc),
        CEREAL_NVP(m_blend),
        CEREAL_NVP(m_type),
        CEREAL_NVP(m_scalerandom),
        CEREAL_NVP(m_randomrotation),
        CEREAL_NVP(m_speed),
        CEREAL_NVP(m_string),
        CEREAL_NVP(m_scalar),
        CEREAL_NVP(m_stringlate),
        CEREAL_NVP(m_filename),
        CEREAL_NVP(m_orbitalvelocity),
        CEREAL_NVP(m_randombuoyancy),
        CEREAL_NVP(m_randomvelocity),
        CEREAL_NVP(m_buoyancy),
        CEREAL_NVP(m_velocity),
        CEREAL_NVP(m_scale),
        CEREAL_NVP(m_latesize),
        CEREAL_NVP(m_particlecolor),
        CEREAL_NVP(m_intensity),
        CEREAL_NVP(usetime),
        CEREAL_NVP(limitime),
        CEREAL_NVP(collsionradius)
    );
}

//日本語にするマクロ
#define J(x) reinterpret_cast<const char*>(x)

//コンストラクタ
CPUParticle::CPUParticle(const char* filename, int num, bool col)
{
    Graphics& graphics = Graphics::Instance();

    //頂点シェーダー
    //インプットレイアウト
    D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "ROTATION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "PARAM", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    CreateVsFromCso(graphics.GetDevice(), "Shader\\CPUParticleVS.cso", m_vertexshader.GetAddressOf(), m_inputlayout.ReleaseAndGetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));

    //ジオメトリックシェーダー
    CreateGsFromCso(graphics.GetDevice(), "Shader\\CPUParticleGS.cso", m_geometryshader.GetAddressOf());

    // ピクセルシェーダー
    CreatePsFromCso(graphics.GetDevice(), "Shader\\CPUParticlePS.cso", m_pixelshader.GetAddressOf());

    m_data = new ParticleData[num];
    ZeroMemory(m_data, sizeof(ParticleData) * num);

    m_numparticle = num;
    m_v = new VERTEX[num];
    ZeroMemory(m_v, sizeof(VERTEX) * num);

    for (int i = 0; i < m_numparticle; ++i) { m_data[i].type = 1; }

    //頂点バッファ作成
    D3D11_BUFFER_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.Usage = D3D11_USAGE_DEFAULT;
    //頂点数分のバッファ
    desc.ByteWidth = sizeof(VERTEX) * m_numparticle;
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    desc.CPUAccessFlags = 0;
    HRESULT hr = graphics.GetDevice()->CreateBuffer(&desc, NULL, m_vertexbuffer.GetAddressOf());
    assert(SUCCEEDED(hr));

    //コンスタントバッファ設定
    m_cc = std::make_unique<ConstantBuffer<CPUParticleConstant>>(graphics.GetDevice());

    //ファイル読み込み処理
    if (filename)
    {
        Desirialize(filename);
        D3D11_TEXTURE2D_DESC texture2d_desc{};
        LoadTextureFromFile(Graphics::Instance().GetDevice(), m_scp.m_filename.c_str(), m_shaderresourceview.GetAddressOf(), &texture2d_desc);
    }

    //コライダーフラグ
    col = collider;
}

//デストラクタ
CPUParticle::~CPUParticle()
{
    delete[] m_data;
    delete[] m_v;
}

//更新処理
void CPUParticle::Update(float elapsedTime)
{
    // パーティクル更新
    if (m_pause)
    {
        if (m_active && m_scp.usetime)
        {
            timer += elapsedTime;
        }
        if (timer > m_scp.limitime)
        {
            m_active = false;
            timer = 0.0f;
        }

        for (int i = 0; i < m_numparticle; i++)
        {
            if (m_data[i].type == -1) continue; // 非表示のパーティクルをスキップ

            if (m_data[i].type < 0) continue;

            //寿命時間の割合を求める
            float lifelate = 1 - (m_data[i].timer / m_scp.m_lifetime);

            m_data[i].vx += m_data[i].ax * elapsedTime;
            m_data[i].vy += m_data[i].ay * elapsedTime;
            m_data[i].vz += m_data[i].az * elapsedTime;

            m_data[i].x += m_data[i].vx * m_scp.m_string * Mathf::Lerp(m_scp.m_stringlate.x, m_scp.m_stringlate.y, lifelate) * elapsedTime;
            m_data[i].y += m_data[i].vy * m_scp.m_string * Mathf::Lerp(m_scp.m_stringlate.x, m_scp.m_stringlate.y, lifelate) * elapsedTime;
            m_data[i].z += m_data[i].vz * m_scp.m_string * Mathf::Lerp(m_scp.m_stringlate.x, m_scp.m_stringlate.y, lifelate) * elapsedTime;

            m_data[i].timer -= elapsedTime;
            m_data[i].alpha = sqrtf(m_data[i].timer);

            if (m_data[i].timer <= 0) m_data[i].type = -1;
        }

        //アニメーション
        for (int i = 0; i < m_numparticle; i++)
        {
            if (m_data[i].type < 0) continue;
            m_data[i].type += elapsedTime * m_scp.m_animationspeed; // speedコマ/秒(ここは後々変数になるかも)
        }

        //パーティクルの動き
        ParticleMove(elapsedTime);
    }
}

//描画処理
void CPUParticle::Render()
{
    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();

    //シェーダーの設定
    dc->VSSetShader(m_vertexshader.Get(), nullptr, 0);
    dc->PSSetShader(m_pixelshader.Get(), nullptr, 0);
    dc->GSSetShader(m_geometryshader.Get(), nullptr, 0);
    dc->IASetInputLayout(m_inputlayout.Get());

    //ブレンドステート設定
    const float BlendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
    dc->OMSetBlendState(graphics.GetBlendState(static_cast<BLENDSTATE>(m_scp.m_blend)), BlendFactor, 0xFFFFFFFF);

    //ラスタライザの設定
    dc->RSSetState(graphics.GetRasterizerState(RASTERIZERSTATE::SOLID_CULL_NONE));

    //デプスステンシルステート設定
    dc->OMSetDepthStencilState(graphics.GetDepthStencilState(DEPTHSTATE::ZT_ON_ZW_OFF), 0);

    //プリミティブトポロジー設定
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

    //テクスチャの設定
    dc->PSSetShaderResources(0, 1, m_shaderresourceview.GetAddressOf());

    //コンスタントバッファの更新
    m_cc->data.particlecolor = m_scp.m_particlecolor;
    m_cc->data.intensity = m_scp.m_intensity;
    m_cc->Activate(dc, (int)CB_INDEX::CPU_PARTICLE, false, true, false, false, false, false);

    //パーティクル発生数
    int n = 0;
    for (int i = 0; i < m_numparticle; ++i)
    {
        if (m_data[i].type < 0)continue;

        m_v[n].Pos.x = m_data[i].x;
        m_v[n].Pos.y = m_data[i].y;
        m_v[n].Pos.z = m_data[i].z;
        m_v[n].Rotate.x = m_data[i].rx;
        m_v[n].Rotate.y = m_data[i].ry;
        m_v[n].Rotate.z = m_data[i].rz;
        m_v[n].Rotate.w = m_data[i].rw;
        m_v[n].Tex.x = m_data[i].w;
        m_v[n].Tex.y = m_data[i].h;
        m_v[n].Color.x = 1;
        m_v[n].Color.y = m_v[n].Color.z = 1.0f;
        m_v[n].Color.w = m_data[i].alpha;
        m_v[n].Param.x = 0;
        m_v[n].Param.y = m_data[i].type;
        m_v[n].Param.z = static_cast<float>(m_scp.m_besidekoma); //横コマ数
        m_v[n].Param.w = static_cast<float>(m_scp.m_verticalkoma); //縦コマ数
        ++n;
    }

    //頂点データ更新
    dc->UpdateSubresource(m_vertexbuffer.Get(), 0, nullptr, m_v, 0, 0);

    //バーテックスバッファをセット
    UINT stride = sizeof(VERTEX);
    UINT offset = 0;
    dc->IASetVertexBuffers(0, 1, m_vertexbuffer.GetAddressOf(), &stride, &offset);

    //レンダリング(インデックス付き)
    dc->Draw(n, 0);

    //デプスステンシルステート設定
    dc->OMSetDepthStencilState(graphics.GetDepthStencilState(DEPTHSTATE::ZT_OFF_ZW_ON), 0);

    //解放してあげる
    dc->VSSetShader(NULL, NULL, 0);
    dc->GSSetShader(NULL, NULL, 0);
    dc->PSSetShader(NULL, NULL, 0);

    //当たり判定描画
    if (collider)
    {
        for (int i = 0; i < m_numparticle; ++i)
        {
            if (m_data[i].type == -1) continue; // 非表示のパーティクルをスキップ

            Graphics::Instance().GetDebugRenderer()->DrawSphere(m_v[i].Pos, m_scp.collsionradius, { 1,0,0,1 });
        }
    }
}

//セット
void CPUParticle::Set(int type, float time, DirectX::XMFLOAT3 p, DirectX::XMFLOAT4 r, DirectX::XMFLOAT3 v, DirectX::XMFLOAT3 f, DirectX::XMFLOAT2 size)
{
    for (int i = 0; i < m_numparticle; i++)
    {
        if (m_data[i].type >= 0) continue;
        m_data[i].type = static_cast<float>(type);
        m_data[i].x = p.x;
        m_data[i].y = p.y;
        m_data[i].z = p.z;
        m_data[i].rx = r.x;
        m_data[i].ry = r.y;
        m_data[i].rz = r.z;
        m_data[i].rw = r.w;
        m_data[i].vx = v.x;
        m_data[i].vy = v.y;
        m_data[i].vz = v.z;
        m_data[i].ax = f.x;
        m_data[i].ay = f.y;
        m_data[i].az = f.z;
        m_data[i].w = size.x;
        m_data[i].h = size.y;
        m_data[i].alpha = 1.0f;
        m_data[i].timer = time;
        break;
    }
}

//パーティクルの動き
void CPUParticle::ParticleMove(float elapsedTime)
{
    if (m_active)
    {
        for (int i = 0; i < m_scp.m_max; i++)
        {
            //位置
            DirectX::XMFLOAT3 position = { GetGameObject()->transform_->GetWorldPosition() };

            //回転
            DirectX::XMFLOAT4 rotation = GetGameObject()->transform_->GetRotation();

            //広がるやつ
            float angle = (rand() % m_scp.m_arc);
            position.x += cos(DirectX::XMConvertToRadians(angle)) * m_scp.m_radius;
            position.z += sin(DirectX::XMConvertToRadians(angle)) * m_scp.m_radius;

            for (int j = 0; j < m_numparticle; j++)
            {
                //寿命時間の割合を求める
                float lifelate = 1 - (m_data[j].timer / m_scp.m_lifetime);

                //回転のやつ
                DirectX::XMFLOAT3 vec = Mathf::Normalize(position - DirectX::XMFLOAT3(m_data[j].x, m_data[j].y, m_data[j].z));
                DirectX::XMFLOAT3 X = Mathf::Cross(vec, DirectX::XMFLOAT3(1, 0, 0));
                DirectX::XMFLOAT3 Y = Mathf::Cross(vec, DirectX::XMFLOAT3(0, 1, 0));
                DirectX::XMFLOAT3 Z = Mathf::Cross(vec, DirectX::XMFLOAT3(0, 0, 1));
                DirectX::XMFLOAT3 orbVelo = { X * m_scp.m_orbitalvelocity.x + Y * m_scp.m_orbitalvelocity.y + Z * m_scp.m_orbitalvelocity.z };
                DirectX::XMFLOAT3 radialvector = -vec * m_scp.m_scaleradius;
                m_data[j].x += (m_scp.m_velocity.x + orbVelo.x + radialvector.x) * m_scp.m_scalar;
                m_data[j].y += (m_scp.m_velocity.y + orbVelo.y + radialvector.y) * m_scp.m_scalar;
                m_data[j].z += (m_scp.m_velocity.z + orbVelo.z + radialvector.z) * m_scp.m_scalar;

                //重力ランダム
                m_data[j].vx += Mathf::RandomRange(0.0f, m_scp.m_randomvelocity.x);
                m_data[j].vy += Mathf::RandomRange(0.0f, m_scp.m_randomvelocity.y);
                m_data[j].vz += Mathf::RandomRange(0.0f, m_scp.m_randomvelocity.z);

                //ベクトルを適用
                float length = sqrt(m_direction.x * m_direction.x + m_direction.y * m_direction.y + m_direction.z * m_direction.z);
                m_direction.x /= length;
                m_direction.y /= length;
                m_direction.z /= length;
                m_data[j].vx += m_direction.x * m_scp.m_speed;
                m_data[j].vy += m_direction.y * m_scp.m_speed;
                m_data[j].vz += m_direction.z * m_scp.m_speed;

                //浮力ランダム
                m_data[j].ax += Mathf::RandomRange(0.0f, m_scp.m_randombuoyancy.x);
                m_data[j].ay += Mathf::RandomRange(0.0f, m_scp.m_randombuoyancy.y);
                m_data[j].az += Mathf::RandomRange(0.0f, m_scp.m_randombuoyancy.z);

                //大きさをラープで制御
                m_data[j].w = Mathf::Lerp(m_scp.m_latesize.x, m_scp.m_latesize.y, lifelate);
                m_data[j].h = Mathf::Lerp(m_scp.m_latesize.x, m_scp.m_latesize.y, lifelate);

                //スケールランダム
                DirectX::XMFLOAT2 latescale = { m_data[j].w, m_data[j].h };
                latescale += Mathf::RandomRange(0.0f, m_scp.m_scalerandom);
                m_data[j].w = Mathf::Lerp(m_scp.m_scale.x, latescale.x, lifelate);
                m_data[j].h = Mathf::Lerp(m_scp.m_scale.y, latescale.y, lifelate);

                //回転ランダム
                m_data[j].rz += Mathf::RandomRange(0.0f, m_scp.m_randomrotation);
            }

            //セットする
            Set(m_scp.m_type, m_scp.m_lifetime, position, rotation, m_scp.m_velocity, m_scp.m_buoyancy, m_scp.m_scale);
        }
    }
}

//imgui
void CPUParticle::OnGUI()
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
        LoadTextureFromFile(Graphics::Instance().GetDevice(), m_scp.m_filename.c_str(), m_shaderresourceview.GetAddressOf(), &texture2d_desc);
    }
    ImGui::SameLine();
    if (ImGui::Button("Reset"))
    {
        Reset();
    }
    //テクスチャロード
    char textureFile[256];
    ZeroMemory(textureFile, sizeof(textureFile));
    ::strncpy_s(textureFile, sizeof(textureFile), m_scp.m_filename.c_str(), sizeof(textureFile));
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
            m_scp.m_filename = relativeTextureFile;
            D3D11_TEXTURE2D_DESC texture2d_desc{};
            LoadTextureFromFile(Graphics::Instance().GetDevice(), m_scp.m_filename.c_str(), m_shaderresourceview.GetAddressOf(), &texture2d_desc);
        }
    }
    ImGui::SameLine();
    ::strncpy_s(textureFile, sizeof(textureFile), m_scp.m_filename.c_str(), sizeof(textureFile));
    if (ImGui::InputText("texture", textureFile, sizeof(textureFile), ImGuiInputTextFlags_EnterReturnsTrue))
    {
        m_scp.m_filename = textureFile;

        char drive[32], dir[256], fullPath[256];
        ::_splitpath_s(textureFile, drive, sizeof(drive), dir, sizeof(dir), nullptr, 0, nullptr, 0);
        ::_makepath_s(fullPath, sizeof(fullPath), drive, dir, textureFile, nullptr);
        D3D11_TEXTURE2D_DESC texture2d_desc{};
        LoadTextureFromFile(Graphics::Instance().GetDevice(), m_scp.m_filename.c_str(), m_shaderresourceview.GetAddressOf(), &texture2d_desc);
    }
    ImGui::Text(J(u8"リソース"));
    ImGui::Image(m_shaderresourceview.Get(), { 256, 256 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
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
    ImGui::Combo("BlendMode", &m_scp.m_blend, BlendName, static_cast<int>(BLENDSTATE::MAX), 10);
    ImGui::Checkbox(J(u8"エフェクト描画"), &m_active);
    ImGui::SameLine();
    ImGui::Checkbox(J(u8"エフェクト停止"), &m_pause);
    ImGui::SetNextItemWidth(80);
    ImGui::InputInt(J(u8"開始コマ"), &m_scp.m_type);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(80);
    ImGui::InputInt(J(u8"縦コマ"), &m_scp.m_verticalkoma);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(80);
    ImGui::InputInt(J(u8"横コマ"), &m_scp.m_besidekoma);
    ImGui::SetNextItemWidth(80);
    ImGui::InputInt(J(u8"ループ回数"), &m_scp.m_max);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(92);
    ImGui::DragFloat(J(u8"アニメーションスピード"), &m_scp.m_animationspeed, 0.1f, 0.0f, 60.0f);
    ImGui::DragFloat2(J(u8"スケール"), &m_scp.m_scale.x, 0.01f, 0);
    if (ImGui::DragFloat(J(u8"全部のスケール"), &m_scp.m_scale.x, 0.01f, 0))
    {
        m_scp.m_scale.y = m_scp.m_scale.x;
    }
    ImGui::Spacing();
    ImGui::ColorEdit4(J(u8"色"), &m_scp.m_particlecolor.x);
    ImGui::DragFloat3(J(u8"輝度"), &m_scp.m_intensity.x, 0.1f, 0.0f, 300.0f);
    ImGui::DragFloat(J(u8"生存時間"), &m_scp.m_lifetime, 0.1f);
    ImGui::Checkbox(J(u8"制限時間を使う"), &m_scp.usetime);
    ImGui::DragFloat(J(u8"時間"), &timer, 0.1f);
    ImGui::DragFloat(J(u8"制限時間"), &m_scp.limitime, 0.1f, 0.0f, 100.0f);
    ImGui::DragFloat3(J(u8"重力方向"), &m_scp.m_velocity.x, 0.1f);
    ImGui::DragFloat3(J(u8"重力ランダム"), &m_scp.m_randomvelocity.x, 0.1f, 0.0f);
    ImGui::DragFloat3(J(u8"浮力"), &m_scp.m_buoyancy.x, 0.1f);
    ImGui::DragFloat3(J(u8"浮力ランダム"), &m_scp.m_randombuoyancy.x, 0.1f, 0.0f);
    ImGui::SetNextItemWidth(90);
    ImGui::DragInt(J(u8"生成角度"), &m_scp.m_arc, 0.1f, 1, 360);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(90);
    ImGui::DragFloat(J(u8"半径"), &m_scp.m_radius, 0.1f);
    ImGui::DragFloat3(J(u8"回転係数"), &m_scp.m_orbitalvelocity.x, 0.1f);
    ImGui::SetNextItemWidth(90);
    ImGui::DragFloat(J(u8"広がる早さ"), &m_scp.m_scalar, 0.1f, 0.1f, 100.0f);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(90);
    ImGui::DragFloat(J(u8"広がる大きさ"), &m_scp.m_scaleradius, 0.1f);
    ImGui::SetNextItemWidth(90);
    ImGui::DragFloat(J(u8"スケールランダム"), &m_scp.m_scalerandom, 0.1f, 0.0f, 200.0f);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(90);
    ImGui::DragFloat(J(u8"Z軸回転ランダム"), &m_scp.m_randomrotation, 0.01f, -0.1f, 0.1f);
    ImGui::SetNextItemWidth(90);
    ImGui::DragFloat(J(u8"生成時の大きさ"), &m_scp.m_latesize.x, 0.01f, 0.0f, 1.0f);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(90);
    ImGui::DragFloat(J(u8"消滅時の大きさ"), &m_scp.m_latesize.y, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat3(J(u8"方向ベクトル"), &m_direction.x, 0.1f);
    ImGui::DragFloat(J(u8"ベクトルに加算するスピード"), &m_scp.m_speed, 0.1f);
    ImGui::DragFloat(J(u8"パーティクルの速さ"), &m_scp.m_string, 0.01f, 1.0f, 10.0f);
    ImGui::SetNextItemWidth(90);
    ImGui::DragFloat(J(u8"パーティクルの最初の速さ"), &m_scp.m_stringlate.x, 0.01f, 1.0f, 10.0f);
    ImGui::SameLine();
    ImGui::SetNextItemWidth(90);
    ImGui::DragFloat(J(u8"パーティクルの最後の速さ"), &m_scp.m_stringlate.y, 0.01f, 1.0f, 10.0f);
    ImGui::DragFloat(u8"当たり判定の半径", &m_scp.collsionradius);
}

//シリアライズ
void CPUParticle::Serialize()
{
    static const char* filter = "Particle Files(*.cpuparticle)\0*.cpuparticle;\0All Files(*.*)\0*.*;\0\0";

    char filename[256] = { 0 };
    DialogResult result = Dialog::SaveFileName(filename, sizeof(filename), filter, nullptr, "cpuparticle", Graphics::Instance().GetHwnd());
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
                    CEREAL_NVP(m_scp)
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
void CPUParticle::Desirialize(const char* filename)
{
    std::ifstream istream(filename, std::ios::binary);
    if (istream.is_open())
    {
        cereal::BinaryInputArchive archive(istream);

        try
        {
            archive
            (
                CEREAL_NVP(m_scp)
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
void CPUParticle::LoadDesirialize()
{
    static const char* filter = "particle Files(*.cpuparticle)\0*.cpuparticle;\0All Files(*.*)\0*.*;\0\0";

    char filename[256] = { 0 };
    DialogResult result = Dialog::OpenFileName(filename, sizeof(filename), filter, nullptr, Graphics::Instance().GetHwnd());
    if (result == DialogResult::OK)
    {
        Desirialize(filename);
    }
}

//リセット
void CPUParticle::Reset()
{
    m_scp.m_animationspeed = 0.1f;
    m_scp.m_max = 1;
    m_scp.m_lifetime = 0.0f;
    m_scp.m_radius = 0.0f;
    m_scp.m_scaleradius = 0.0f;
    m_scp.m_arc = 1;
    m_scp.m_type = 0;
    m_scp.m_scalerandom = 0;
    m_scp.m_randomrotation = 0.0f;
    m_scp.m_speed = 0.0f;
    m_scp.m_string = 1.0f;
    m_scp.m_scalar = 1.0f;
    m_scp.m_stringlate = { 1.0f,1.0f };
    m_scp.m_orbitalvelocity = { 0,0,0 };
    m_scp.m_randombuoyancy = { 0,0,0 };
    m_scp.m_randomvelocity = { 0,0,0 };
    m_scp.m_buoyancy = { 0,0,0 };
    m_scp.m_velocity = { 0,0,0 };
    m_scp.m_scale = { 1,1 };
    m_scp.m_latesize = { 1,1 };
    m_scp.m_particlecolor = { 1,1,1,1 };
    m_scp.m_intensity = { 1,1,1 };
    m_scp.collsionradius = { 0.0f };
}