#include "LightCom.h"
#include "Graphics/Graphics.h"
#include "Component\System\TransformCom.h"
#include "Math\Mathf.h"
#include <cereal/cereal.hpp>
#include <cereal\types\vector.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>
#include <SystemStruct\Dialog.h>
#include <SystemStruct\Logger.h>
#include <fstream>
#include <filesystem>

CEREAL_CLASS_VERSION(Light::LightParameter, 1)

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
void Light::LightParameter::serialize(Archive& archive, int version)
{
    archive
    (
        CEREAL_NVP(power),
        CEREAL_NVP(lighttype),
        CEREAL_NVP(color),
        CEREAL_NVP(position),
        CEREAL_NVP(direction),
        CEREAL_NVP(range),
        CEREAL_NVP(innerCone),
        CEREAL_NVP(outerCone)
    );
}

// コンストラクタ
Light::Light(const char* filename)
{
    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();

    // 定数バッファの作成
    D3D11_BUFFER_DESC desc;
    ::memset(&desc, 0, sizeof(desc));
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    desc.ByteWidth = sizeof(LightCB);
    desc.StructureByteStride = 0;

    // 定数バッファの作成
    HRESULT hr = graphics.GetDevice()->CreateBuffer(&desc, 0, m_lightCb.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

    // ファイルがあれば読み込み
    if (filename)
    {
        Desirialize(filename);
    }
}

// 更新処理
void Light::Update(float elapsedTime)
{
    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();

    // LP を反映して光源の更新
    for (size_t i = 0; i < LP.size(); ++i)
    {
        const auto& param = LP[i];

        switch (static_cast<LightType>(param.lighttype))
        {
        case LightType::Directional:
            cb.directionalLight.color = param.color * param.power;
            cb.directionalLight.direction = { param.direction.x, param.direction.y, param.direction.z, 0.0f };
            break;
        case LightType::Point:
            cb.pointLight[i].color = param.color * param.power;
            cb.pointLight[i].position = { param.position.x, param.position.y, param.position.z, 1.0f };
            cb.pointLight[i].range = param.range;
            break;
        case LightType::Spot:
            cb.spotLight[i].color = param.color * param.power;
            cb.spotLight[i].position = { param.position.x, param.position.y, param.position.z, 1.0f };
            cb.spotLight[i].direction = { param.direction.x, param.direction.y, param.direction.z, 0.0f };
            cb.spotLight[i].range = param.range;
            cb.spotLight[i].innerCorn = param.innerCone;
            cb.spotLight[i].outerCorn = param.outerCone;
            break;
        default:
            assert("Invalid Light Type");
        }
    }

    // 定数バッファの更新
    dc->UpdateSubresource(m_lightCb.Get(), 0, 0, &cb, 0, 0);

    // シェーダーにバインド
    dc->VSSetConstantBuffers((int)CB_INDEX::LIGHT_DATA, 1, m_lightCb.GetAddressOf());
    dc->PSSetConstantBuffers((int)CB_INDEX::LIGHT_DATA, 1, m_lightCb.GetAddressOf());
    dc->GSSetConstantBuffers((int)CB_INDEX::LIGHT_DATA, 1, m_lightCb.GetAddressOf());
    dc->CSSetConstantBuffers((int)CB_INDEX::LIGHT_DATA, 1, m_lightCb.GetAddressOf());

    // デバッグ描画
    DebugPrimitive();
}

// ImGui UI
void Light::OnGUI()
{
    if (ImGui::Button("Save"))
    {
        Serialize();
    }
    ImGui::SameLine();
    if (ImGui::Button("Load"))
    {
        LoadDesirialize();
    }

    if (ImGui::Button("Add Light"))
    {
        // 新しいライトを追加
        LightParameter newLight;
        newLight.lighttype = static_cast<int>(LightType::Directional); // デフォルトはDirectional
        newLight.power = 1.0f;
        newLight.color = { 1.0f, 1.0f, 1.0f,1.0f }; // 白色
        newLight.position = { 0.0f, 0.0f, 0.0f };
        newLight.direction = { 0.0f, -1.0f, -1.0f }; // デフォルト方向
        newLight.range = 10.0f;
        newLight.innerCone = 0.5f;
        newLight.outerCone = 1.0f;
        LP.push_back(newLight);
    }

    for (size_t i = 0; i < LP.size(); ++i)
    {
        ImGui::PushID(static_cast<int>(i)); // 複数のライトを区別するためのID
        ImGui::Separator();

        // ライトタイプ選択
        constexpr const char* lightTypeNames[] = { "Directional", "Point", "Spot" };
        ImGui::Combo("Type", reinterpret_cast<int*>(&LP[i].lighttype), lightTypeNames, static_cast<int>(LightType::MAX));

        // 共通プロパティ
        ImGui::ColorEdit3("Color", &LP[i].color.x);
        ImGui::DragFloat("Power", &LP[i].power, 0.05f, 0.0f, 10.0f);

        // タイプ別プロパティ
        switch (static_cast<LightType>(LP[i].lighttype))
        {
        case LightType::Directional:
            ImGui::DragFloat3("Direction", &LP[i].direction.x, 0.01f, -1.0f, 1.0f);
            break;
        case LightType::Point:
            ImGui::DragFloat3("Position", &LP[i].position.x, 0.1f);
            ImGui::DragFloat("Range", &LP[i].range, 0.1f, 0.0f, FLT_MAX);
            break;
        case LightType::Spot:
            ImGui::DragFloat3("Position", &LP[i].position.x, 0.1f);
            ImGui::DragFloat3("Direction", &LP[i].direction.x, 0.01f, -1.0f, 1.0f);
            ImGui::DragFloat("Range", &LP[i].range, 0.1f, 0.0f, FLT_MAX);
            ImGui::SliderFloat("Inner Cone", &LP[i].innerCone, 0.0f, 1.0f);
            ImGui::SliderFloat("Outer Cone", &LP[i].outerCone, 0.0f, 1.0f);
            break;
        }

        // ライト削除ボタン
        if (ImGui::Button("Remove Light"))
        {
            LP.erase(LP.begin() + i);
            ImGui::PopID();
            continue; // 現在のライトを削除したので次に進む
        }

        ImGui::PopID();
    }
}

// デバッグ描画
void Light::DebugPrimitive()
{
    DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();
    LineRenderer* lineRenderer = Graphics::Instance().GetLineRenderer();
    static constexpr int SplitCount = 16;

    // ライトの種類によって処理を分岐
    for (size_t i = 0; i < LP.size(); ++i)
    {
        const auto& param = LP[i];

        switch (static_cast<LightType>(param.lighttype))
        {
        case LightType::Directional:

            // Directionalライトの場合、方向を示す線を描画
            lineRenderer->AddVertex({ 0, 0.4f, 0 }, param.color); // ライトの位置（ライト源）

            // ライトの方向に基づいて線の終点を計算
            DirectX::XMFLOAT3 lightDirection = { param.direction.x, param.direction.y, param.direction.z };
            DirectX::XMVECTOR lightVec = DirectX::XMLoadFloat3(&lightDirection);
            lightVec = DirectX::XMVectorScale(lightVec, -250);  // ライトの方向を強調

            DirectX::XMFLOAT3 lightEnd;
            DirectX::XMStoreFloat3(&lightEnd, lightVec);

            lineRenderer->AddVertex(lightEnd, param.color); // ライト方向を示す線の終点
            break;

        case LightType::Point:

            // Pointライトの場合、光源位置を球で表示
            debugRenderer->DrawSphere({ param.position.x, param.position.y, param.position.z },
                param.range, param.color); // 範囲も表示
            break;

        case LightType::Spot:

            // 方向の正規化
            DirectX::XMVECTOR Direction = DirectX::XMLoadFloat3(&param.direction);
            float len;
            DirectX::XMStoreFloat(&len, DirectX::XMVector3Length(Direction));
            if (len <= 0.00001f)
                break;
            Direction = DirectX::XMVector3Normalize(Direction);

            // 軸算出
            DirectX::XMFLOAT3 dir;
            DirectX::XMStoreFloat3(&dir, Direction);
            DirectX::XMVECTOR Work = fabs(dir.y) == 1 ? DirectX::XMVectorSet(1, 0, 0, 0)
                : DirectX::XMVectorSet(0, 1, 0, 0);
            DirectX::XMVECTOR XAxis = DirectX::XMVector3Cross(Direction, Work);
            DirectX::XMVECTOR YAxis = DirectX::XMVector3Cross(XAxis, Direction);
            XAxis = DirectX::XMVector3Cross(Direction, YAxis);

            for (int u = 0; u < SplitCount; u++)
            {
                float s = static_cast<float>(u) / static_cast<float>(SplitCount);
                float r = -DirectX::XM_PI + DirectX::XM_2PI * s;

                // 回転行列算出
                DirectX::XMMATRIX RotationZ = DirectX::XMMatrixRotationAxis(Direction, r);

                // 線を算出
                DirectX::XMFLOAT3 OldPoint;
                {
                    DirectX::XMVECTOR Point = Direction;
                    DirectX::XMMATRIX Rotation = DirectX::XMMatrixRotationAxis(XAxis, acosf(param.outerCone)) * RotationZ;
                    Point = DirectX::XMVectorMultiply(Point, DirectX::XMVectorSet(param.range, param.range, param.range, 0));
                    Point = DirectX::XMVector3TransformCoord(Point, Rotation);
                    Point = DirectX::XMVectorAdd(Point, DirectX::XMLoadFloat3(&param.position));

                    DirectX::XMFLOAT3 pos;
                    DirectX::XMStoreFloat3(&pos, Point);
                    lineRenderer->AddVertex(param.position, param.color);
                    lineRenderer->AddVertex(pos, param.color);
                    OldPoint = pos;
                }

                // 球面を算出
                for (int v = 0; v <= SplitCount; ++v)
                {
                    float s = static_cast<float>(v) / static_cast<float>(SplitCount);
                    float a = param.outerCone + (1.0f - param.outerCone) * s;
                    DirectX::XMVECTOR Point = Direction;
                    DirectX::XMMATRIX Rotation = DirectX::XMMatrixRotationAxis(XAxis, acosf(a)) * RotationZ;
                    Point = DirectX::XMVectorMultiply(Point, DirectX::XMVectorSet(param.range, param.range, param.range, 0));
                    Point = DirectX::XMVector3TransformCoord(Point, Rotation);
                    Point = DirectX::XMVectorAdd(Point, DirectX::XMLoadFloat3(&param.position));

                    DirectX::XMFLOAT3 pos;
                    DirectX::XMStoreFloat3(&pos, Point);
                    lineRenderer->AddVertex(OldPoint, param.color);
                    lineRenderer->AddVertex(pos, param.color);
                    OldPoint = pos;
                }
            }
            break;
        default:
            // 無効なライトタイプの場合
            assert("Invalid Light Type");
            break;
        }
    }
}

//シリアライズ
void Light::Serialize()
{
    static const char* filter = "Light Files(*.light)\0*.light;\0All Files(*.*)\0*.*;\0\0";

    char filename[256] = { 0 };
    DialogResult result = Dialog::SaveFileName(filename, sizeof(filename), filter, nullptr, "light", Graphics::Instance().GetHwnd());
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
                    CEREAL_NVP(LP)
                );
            }
            catch (...)
            {
                LOG("light deserialize failed.\n%s\n", filename);
                return;
            }
        }
    }
}

//デシリアライズ
void Light::Desirialize(const char* filename)
{
    std::ifstream istream(filename, std::ios::binary);
    if (istream.is_open())
    {
        cereal::BinaryInputArchive archive(istream);

        try
        {
            archive
            (
                CEREAL_NVP(LP)
            );
        }
        catch (...)
        {
            LOG("light deserialize failed.\n%s\n", filename);
            return;
        }
    }
}

//読み込み
void Light::LoadDesirialize()
{
    static const char* filter = "Light Files(*.light)\0*.light;\0All Files(*.*)\0*.*;\0\0";

    char filename[256] = { 0 };
    DialogResult result = Dialog::OpenFileName(filename, sizeof(filename), filter, nullptr, Graphics::Instance().GetHwnd());
    if (result == DialogResult::OK)
    {
        Desirialize(filename);
    }
}