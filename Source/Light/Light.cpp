#include  "Graphics/Graphics.h"
#include  "Light.h"
#include  <imgui.h>
#include  <string>

Light::Light(LightType lightType)
    : lightType(lightType)
{
}

void Light::PushRenderContext(LightCB& cb)
{
    // ìoò^Ç≥ÇÍÇƒÇ¢ÇÈåıåπÇÃèÓïÒÇê›íË
    switch (lightType)
    {
    case	LightType::Directional:
    {
        DirectX::XMVECTOR Direction = DirectX::XMLoadFloat3(&direction);
        DirectX::XMVECTOR COLOR = DirectX::XMLoadFloat4(&color);
        DirectX::XMFLOAT4 c = {};
        DirectX::XMStoreFloat4(&c, DirectX::XMVectorScale(COLOR, power));
        DirectX::XMStoreFloat3(&direction, DirectX::XMVector3Normalize(Direction));

        cb.directionalLight.direction.x = direction.x;
        cb.directionalLight.direction.y = direction.y;
        cb.directionalLight.direction.z = direction.z;
        cb.directionalLight.direction.w = 0.0f;
        cb.directionalLight.color = c;
        break;
    }
    //case	LightType::Point:
    //{
    //  if (rc.pointLightCount >= PointLightMax)
    //    break;
    //  rc.pointLightData[rc.pointLightCount].position.x = position.x;
    //  rc.pointLightData[rc.pointLightCount].position.y = position.y;
    //  rc.pointLightData[rc.pointLightCount].position.z = position.z;
    //  rc.pointLightData[rc.pointLightCount].position.w = 1.0f;
    //  rc.pointLightData[rc.pointLightCount].color = color * power;
    //  rc.pointLightData[rc.pointLightCount].range = range;
    //  ++rc.pointLightCount;
    //  break;
    //}
    //case	LightType::Spot:
    //{
    //  if (rc.spotLightCount >= SpotLightMax)
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
    //  break;
    //}

    default:
        assert(0);
    }
}

void Light::DrawDebugGUI()
{
    // é©ìÆÇ≈ImGuiÇÃÉâÉxÉãÇëŒâûÇµÇΩÇ‡ÇÃÇ…ìKópÇ∑ÇÈèàóù
    static const char* lightTypeName[] =
    {
      "Directional",
      "Point",
      "Spot",
    };

    if (ImGui::TreeNode(lightTypeName[static_cast<int>(lightType)]))
    {
        switch (lightType)
        {
        case	LightType::Directional:
        {
            if (ImGui::SliderFloat3("direction", &direction.x, -1.0f, 1.0f))
            {
                float l = sqrtf(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
            }
            ImGui::ColorEdit3("color", &color.x, ImGuiColorEditFlags_::ImGuiColorEditFlags_Float | ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_NoAlpha);
            ImGui::DragFloat("power", &power, 0.05f, 0, 10);

            if (ImGui::Button("reset")) {
                direction = { 0, -1, -1 };
                color = { 1,1,1,1 };
            }
            break;
        }
        case	LightType::Point:
        {
            ImGui::DragFloat3("position", &position.x);
            ImGui::ColorEdit3("color", &color.x);
            ImGui::DragFloat("range", &range, 0.1f, 0, FLT_MAX);
            ImGui::DragFloat("power", &power, 0.05f, 0, 5);

            if (ImGui::Button("reset")) {
                position = { 20, 10, 0 };
                color = { 1,1,1,1 };
                range = 20.0f;
            }
            break;
        }
        case LightType::Spot:
        {
            ImGui::DragFloat3("position", &position.x);
            ImGui::DragFloat3("direction", &direction.x, 0.01f);
            DirectX::XMStoreFloat3(&direction,
                DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&direction)));
            ImGui::ColorEdit3("color", &color.x);
            ImGui::DragFloat("power", &power, 0.05f, 0, 5);
            ImGui::DragFloat("range", &range, 0.1f, 0, FLT_MAX);
            ImGui::SliderFloat("innerCorn", &innerCorn, 0, 1.0f);
            ImGui::SliderFloat("outerCorn", &outerCorn, 0, 1.0f);

            if (ImGui::Button("reset")) {
                direction = { +1, -1, 0 };
                position = { -30, 20, 0 };
                color = { 1,1,1,1 };
                range = 40.0f;
                innerCorn = 0.99f;
                outerCorn = 0.9f;
            }
            break;
        }
        }
        ImGui::TreePop();
    }
}

void Light::DrawDebugPrimitive()
{
    DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();
    LineRenderer* lineRenderer = Graphics::Instance().GetLineRenderer();

    switch (lightType)
    {
    case LightType::Directional:
    {
        lineRenderer->AddVertex({ 0,0.4f,0 }, color);

        DirectX::XMVECTOR lightVec = DirectX::XMLoadFloat3(&direction);
        lightVec = DirectX::XMVectorScale(lightVec, -250);

        DirectX::XMFLOAT3 lightDirection;
        DirectX::XMStoreFloat3(&lightDirection, lightVec);

        lineRenderer->AddVertex(lightDirection, color);

        break;
    }
    case LightType::Point:
    {
        //	ì_åıåπÇÕëSï˚à Ç…åıÇï˙éÀÇ∑ÇÈåıåπÇ»ÇÃÇ≈ãÖëÃÇï`âÊÇ∑ÇÈÅB
        debugRenderer->DrawSphere(position, range, color);
        break;
    }
    case LightType::Spot:
    {
        DirectX::XMVECTOR	Direction = DirectX::XMLoadFloat3(&direction);
        float len;
        DirectX::XMStoreFloat(&len, DirectX::XMVector3Length(Direction));
        if (len <= 0.00001f)
            break;
        Direction = DirectX::XMVector3Normalize(Direction);

        //	é≤éZèo
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
            // âÒì]çsóÒéZèo
            DirectX::XMMATRIX	RotationZ = DirectX::XMMatrixRotationAxis(Direction, r);
            // ê¸ÇéZèo
            DirectX::XMFLOAT3	OldPoint;
            {
                DirectX::XMVECTOR	Point = Direction;
                DirectX::XMMATRIX	Rotation = DirectX::XMMatrixRotationAxis(XAxis, acosf(outerCorn))
                    * RotationZ;
                Point = DirectX::XMVectorMultiply(Point, DirectX::XMVectorSet(range, range, range, 0));
                Point = DirectX::XMVector3TransformCoord(Point, Rotation);
                Point = DirectX::XMVectorAdd(Point, DirectX::XMLoadFloat3(&position));
                DirectX::XMFLOAT3	pos;
                DirectX::XMStoreFloat3(&pos, Point);
                lineRenderer->AddVertex(position, color);
                lineRenderer->AddVertex(pos, color);
                OldPoint = pos;
            }
            // ãÖñ ÇéZèo
            for (int v = 0; v <= SplitCount; ++v)
            {
                float s = static_cast<float>(v) / static_cast<float>(SplitCount);
                float a = outerCorn + (1.0f - outerCorn) * s;
                DirectX::XMVECTOR	Point = Direction;
                DirectX::XMMATRIX	Rotation = DirectX::XMMatrixRotationAxis(XAxis, acosf(a))
                    * RotationZ;
                Point = DirectX::XMVectorMultiply(Point, DirectX::XMVectorSet(range, range, range, 0));
                Point = DirectX::XMVector3TransformCoord(Point, Rotation);
                Point = DirectX::XMVectorAdd(Point, DirectX::XMLoadFloat3(&position));
                DirectX::XMFLOAT3	pos;
                DirectX::XMStoreFloat3(&pos, Point);
                lineRenderer->AddVertex(OldPoint, color);
                lineRenderer->AddVertex(pos, color);
                OldPoint = pos;
            }
        }
        break;
    }
    }
}