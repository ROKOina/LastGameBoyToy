#include "LightCom.h"
#include "Graphics/Graphics.h"
#include "Component\System\TransformCom.h"
#include "Math\Mathf.h"
#include <cereal/cereal.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>
#include <SystemStruct\Dialog.h>
#include <SystemStruct\Logger.h>
#include <fstream>
#include <filesystem>

CEREAL_CLASS_VERSION(Light::DirectionalLightData, 1)
CEREAL_CLASS_VERSION(Light::LightCB, 1)
CEREAL_CLASS_VERSION(Light::LightParameter, 1)
CEREAL_CLASS_VERSION(Light::PointLightData, 1)
CEREAL_CLASS_VERSION(Light::SpotLightData, 1)

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
void Light::DirectionalLightData::serialize(Archive& archive, int version)
{
    archive
    (
        CEREAL_NVP(direction),
        CEREAL_NVP(color)
    );
}

template<class Archive>
void Light::LightCB::serialize(Archive& archive, int version)
{
    archive
    (
        CEREAL_NVP(directionalLight),
        CEREAL_NVP(pointLight),
        CEREAL_NVP(spotLight)
    );
}

template<class Archive>
void Light::PointLightData::serialize(Archive& archive, int version)
{
    archive
    (
        CEREAL_NVP(position),
        CEREAL_NVP(color),
        CEREAL_NVP(range)
    );
}

template<class Archive>
void Light::SpotLightData::serialize(Archive& archive, int version)
{
    archive
    (
        CEREAL_NVP(position),
        CEREAL_NVP(direction),
        CEREAL_NVP(range),
        CEREAL_NVP(innerCorn),
        CEREAL_NVP(outerCorn)
    );
}

template<class Archive>
void Light::LightParameter::serialize(Archive& archive, int version)
{
    archive
    (
        CEREAL_NVP(power),
        CEREAL_NVP(lighttype)
    );
}

//コンストラクタ
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

    //コンスタントバッファの作成
    HRESULT hr = graphics.GetDevice()->CreateBuffer(&desc, 0, m_lightCb.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

    //ファイルがあれば読み込み
    if (filename)
    {
        Desirialize(filename);
    }
}

//初期設定
void Light::Start()
{
    directioncolor = cb.directionalLight.color;
    directioncolor = cb.pointLight[0].color;
    directioncolor = cb.spotLight[0].color;
}

//更新処理
void Light::Update(float elapsedTime)
{
    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();

    //ライトタイプによって変更
    m_lightType = static_cast<LightType>(LP.lighttype);
    LP.lighttype = static_cast<int>(m_lightType);

    // 登録されている光源の情報を設定
    switch (m_lightType)
    {
    case	LightType::Directional:
    {
        cb.directionalLight.color = directioncolor * LP.power;
        break;
    }
    case	LightType::Point:
    {
        if (pointLightCount >= POINT_LIGHT_MAX)
            break;
        cb.pointLight[pointLightCount].position.x = position.x;
        cb.pointLight[pointLightCount].position.y = position.y;
        cb.pointLight[pointLightCount].position.z = position.z;
        cb.pointLight[pointLightCount].position.w = 1.0f;
        cb.pointLight[pointLightCount].color = directioncolor * LP.power;
        cb.pointLight[pointLightCount].range = range;
        ++pointLightCount;
        break;
    }
    case	LightType::Spot:
    {
        //if (rc.spotLightCount >= SpotLightMax)
      //    break;
      //  DirectX::XMVECTOR Direction = DirectX::XMLoadFloat3(&direction);
      //  DirectX::XMStoreFloat3(&direction, DirectX::XMVector3Normalize(Direction));

      //  rc.spotLightData[rc.spotLightCount].position.x = position.x;
      //  rc.spotLightData[rc.spotLightCount].position.y = position.y;

      //  rc.spotLightData[rc.spotLightCount].position.z = position.z;
      //  rc.spotLightData[rc.spotLightCount].position.w = 1.0f;
      //  rc.spotLightData[rc.spotLightCount].direction.x = direction.x;
      //  rc.spotLightData[rc.spotLightCount].direction.y = direction.y;
      //  rc.spotLightData[rc.spotLightCount].direction.z = direction.z;
      //  rc.spotLightData[rc.spotLightCount].direction.w = 0.0f;
      //  rc.spotLightData[rc.spotLightCount].color = color * power;
      //  rc.spotLightData[rc.spotLightCount].range = range;
      //  rc.spotLightData[rc.spotLightCount].innerCorn = innerCorn;
      //  rc.spotLightData[rc.spotLightCount].outerCorn = outerCorn;
      //  ++rc.spotLightCount;

        cb.spotLight[0].color = directioncolor * LP.power;

        break;
    }

    default:
        assert("どのLightでもありません");
    }

    // 定数バッファ設定
    dc->VSSetConstantBuffers((int)CB_INDEX::LIGHT_DATA, 1, m_lightCb.GetAddressOf());
    dc->PSSetConstantBuffers((int)CB_INDEX::LIGHT_DATA, 1, m_lightCb.GetAddressOf());
    dc->CSSetConstantBuffers((int)CB_INDEX::LIGHT_DATA, 1, m_lightCb.GetAddressOf());
    dc->GSSetConstantBuffers((int)CB_INDEX::LIGHT_DATA, 1, m_lightCb.GetAddressOf());

    //定数バッファの更新
    dc->UpdateSubresource(m_lightCb.Get(), 0, 0, &cb, 0, 0);

    //デバッグ描画
    DebugPrimitive();
}

//imgui
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

    constexpr const char* lightTypeName[] = { "Directional", "Point","Spot" };
    static_assert(ARRAYSIZE(lightTypeName) == static_cast<int>(LightType::MAX), "lightTypeName Size Error!");
    ImGui::Combo((char*)u8"ライトタイプ", &LP.lighttype, lightTypeName, static_cast<int>(LightType::MAX));
    m_lightType = static_cast<LightType>(LP.lighttype);
    LP.lighttype = static_cast<int>(m_lightType);

    switch (m_lightType)
    {
    case	LightType::Directional:
    {
        if (ImGui::SliderFloat3((char*)u8"方向", &cb.directionalLight.direction.x, -1.0f, 1.0f))
        {
            float l = sqrtf(cb.directionalLight.direction.x * cb.directionalLight.direction.x + cb.directionalLight.direction.y * cb.directionalLight.direction.y + cb.directionalLight.direction.z * cb.directionalLight.direction.z);
        }
        ImGui::ColorEdit3((char*)u8"色", &directioncolor.x, ImGuiColorEditFlags_::ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_NoAlpha);
        ImGui::DragFloat((char*)u8"光の強さ", &LP.power, 0.05f, 0, 10);
        break;
    }

    case	LightType::Point:
    {
        ImGui::DragFloat3((char*)u8"位置", &cb.pointLight[0].position.x);
        ImGui::ColorEdit3((char*)u8"色", &directioncolor.x, ImGuiColorEditFlags_::ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_NoAlpha);
        ImGui::DragFloat((char*)u8"大きさ", &cb.pointLight[0].range, 0.1f, 0, FLT_MAX);
        ImGui::DragFloat((char*)u8"光の強さ", &LP.power, 0.05f, 0, 10);

        break;
    }

    case LightType::Spot:
    {
        ImGui::DragFloat3((char*)u8"位置", &cb.spotLight[0].position.x);
        ImGui::DragFloat3((char*)u8"方向", &cb.spotLight[0].direction.x, 0.01f);
        DirectX::XMStoreFloat4(&cb.spotLight[0].direction, DirectX::XMVector3Normalize(DirectX::XMLoadFloat4(&cb.spotLight[0].direction)));
        ImGui::ColorEdit3((char*)u8"色", &directioncolor.x, ImGuiColorEditFlags_::ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_NoAlpha);
        ImGui::DragFloat((char*)u8"光の強さ", &LP.power, 0.05f, 0, 10);
        ImGui::DragFloat((char*)u8"大きさ", &cb.spotLight[0].range, 0.1f, 0, FLT_MAX);
        ImGui::SliderFloat((char*)u8"インナー角度", &cb.spotLight[0].innerCorn, 0, 1.0f);
        ImGui::SliderFloat((char*)u8"アウター角度", &cb.spotLight[0].outerCorn, 0, 1.0f);

        break;
    }
    }
}

//デバッグ描画
void Light::DebugPrimitive()
{
    DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();
    LineRenderer* lineRenderer = Graphics::Instance().GetLineRenderer();

    switch (m_lightType)
    {
    case LightType::Directional:
    {
        lineRenderer->AddVertex({ 0,0.4f,0 }, cb.directionalLight.color);

        DirectX::XMFLOAT3 directionallight = { cb.directionalLight.direction.x,cb.directionalLight.direction.y,cb.directionalLight.direction.z };
        DirectX::XMVECTOR lightVec = DirectX::XMLoadFloat3(&directionallight);
        lightVec = DirectX::XMVectorScale(lightVec, -250);

        DirectX::XMFLOAT3 lightDirection;
        DirectX::XMStoreFloat3(&lightDirection, lightVec);

        lineRenderer->AddVertex(lightDirection, cb.directionalLight.color);

        break;
    }
    case LightType::Point:
    {
        //	点光源は全方位に光を放射する光源なので球体を描画する。
        DirectX::XMFLOAT3 pos = { cb.pointLight[0].position.x,cb.pointLight[0].position.y,cb.pointLight[0].position.z };
        debugRenderer->DrawSphere(pos, cb.pointLight[0].range, cb.pointLight[0].color);
        break;
    }
    case LightType::Spot:
    {
        DirectX::XMVECTOR	Direction = DirectX::XMLoadFloat4(&cb.spotLight[0].direction);
        float len;
        DirectX::XMStoreFloat(&len, DirectX::XMVector3Length(Direction));
        if (len <= 0.00001f)
            break;
        Direction = DirectX::XMVector3Normalize(Direction);

        //	軸算出
        DirectX::XMFLOAT3 dir;
        DirectX::XMStoreFloat3(&dir, Direction);
        DirectX::XMVECTOR Work = fabs(dir.y) == 1 ? DirectX::XMVectorSet(1, 0, 0, 0)
            : DirectX::XMVectorSet(0, 1, 0, 0);
        DirectX::XMVECTOR	XAxis = DirectX::XMVector3Cross(Direction, Work);
        DirectX::XMVECTOR	YAxis = DirectX::XMVector3Cross(XAxis, Direction);
        XAxis = DirectX::XMVector3Cross(Direction, YAxis);

        const int SplitCount = 16;
        for (int u = 0; u < SplitCount; u++)
        {
            float s = static_cast<float>(u) / static_cast<float>(SplitCount);
            float r = -DirectX::XM_PI + DirectX::XM_2PI * s;
            // 回転行列算出
            DirectX::XMMATRIX	RotationZ = DirectX::XMMatrixRotationAxis(Direction, r);
            // 線を算出
            DirectX::XMFLOAT3	OldPoint;
            {
                DirectX::XMVECTOR	Point = Direction;
                DirectX::XMMATRIX	Rotation = DirectX::XMMatrixRotationAxis(XAxis, acosf(cb.spotLight[0].outerCorn))
                    * RotationZ;
                Point = DirectX::XMVectorMultiply(Point, DirectX::XMVectorSet(cb.spotLight[0].range, cb.spotLight[0].range, cb.spotLight[0].range, 0));
                Point = DirectX::XMVector3TransformCoord(Point, Rotation);
                Point = DirectX::XMVectorAdd(Point, DirectX::XMLoadFloat4(&cb.spotLight[0].position));
                DirectX::XMFLOAT3	pos;
                DirectX::XMStoreFloat3(&pos, Point);
                DirectX::XMFLOAT3 pos1 = { cb.spotLight[0].position.x,cb.spotLight[0].position.y,cb.spotLight[0].position.z };
                lineRenderer->AddVertex(pos1, cb.spotLight[0].color);
                lineRenderer->AddVertex(pos1, cb.spotLight[0].color);
                OldPoint = pos;
            }
            // 球面を算出
            for (int v = 0; v <= SplitCount; ++v)
            {
                float s = static_cast<float>(v) / static_cast<float>(SplitCount);
                float a = cb.spotLight[0].outerCorn + (1.0f - cb.spotLight[0].outerCorn) * s;
                DirectX::XMVECTOR	Point = Direction;
                DirectX::XMMATRIX	Rotation = DirectX::XMMatrixRotationAxis(XAxis, acosf(a))
                    * RotationZ;
                Point = DirectX::XMVectorMultiply(Point, DirectX::XMVectorSet(cb.spotLight[0].range, cb.spotLight[0].range, cb.spotLight[0].range, 0));
                Point = DirectX::XMVector3TransformCoord(Point, Rotation);
                Point = DirectX::XMVectorAdd(Point, DirectX::XMLoadFloat4(&cb.spotLight[0].position));
                DirectX::XMFLOAT3	pos;
                DirectX::XMStoreFloat3(&pos, Point);
                lineRenderer->AddVertex(OldPoint, cb.spotLight[0].color);
                lineRenderer->AddVertex(pos, cb.spotLight[0].color);
                OldPoint = pos;
            }
        }
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
                    CEREAL_NVP(cb),
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
                CEREAL_NVP(cb),
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
        directioncolor = cb.directionalLight.color;
        directioncolor = cb.pointLight[0].color;
        directioncolor = cb.spotLight[0].color;
    }
}