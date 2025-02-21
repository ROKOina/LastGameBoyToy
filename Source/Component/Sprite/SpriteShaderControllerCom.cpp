#include "SpriteShaderControllerCom.h"
#include "SpriteCom.h"

SpriteShaderControllerCom::SpriteShaderControllerCom()
{
    HRESULT hr = S_OK;
    ID3D11Device* device = Graphics::Instance().GetDevice();

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
}

void SpriteShaderControllerCom::Start()
{
    GetGameObject()->GetComponent<SpriteCom>()->SetDrawStart([&]() {DrawStart(); });
}

void SpriteShaderControllerCom::DrawStart()
{
    Graphics& Graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = Graphics.GetDeviceContext();
    dc->VSSetConstantBuffers(0, 1, m_constantbuffer.GetAddressOf());
    dc->PSSetConstantBuffers(0, 1, m_constantbuffer.GetAddressOf());
    dc->UpdateSubresource(m_constantbuffer.Get(), 0, 0, &m_constants, 0, 0);
}

void SpriteShaderControllerCom::OnGUI()
{
    ImGui::DragFloat2((char*)u8"UVスクロール", &m_constants.uvscroll.x, 0.1f);
    ImGui::ColorEdit3((char*)u8"ブラー色", &m_constants.blurcolor.x);
    ImGui::DragFloat((char*)u8"ルミナンス", &m_constants.luminance, 0.1f, 0.0f, 10.0f);
    ImGui::DragFloat((char*)u8"ブラー距離", &m_constants.blurdistance, 0.1f, 0.0f, 20.0f);
    ImGui::DragFloat((char*)u8"ブラーパワー", &m_constants.blurpower, 0.1f, 0.0f, 1.0f);
    ImGui::ColorEdit3((char*)u8"ディゾルブ色", &m_constants.edgecolor.x);
    ImGui::DragFloat((char*)u8"クリップ時間", &m_constants.cliptime, 0.1f, 0.0f, 1.0f);
    ImGui::DragFloat((char*)u8"ディゾルブ量", &m_constants.edgethreshold, 0.1f, 0.0f, 1.0f);
    ImGui::DragFloat((char*)u8"縁オフセット", &m_constants.edgeoffset, 0.1f, 0.0f, 1.0f);
    ImGui::DragFloat((char*)u8"アニメーション速度", &m_constants.framerate, 0.1f, 0.0f, 60.0f);
    ImGui::InputInt((char*)u8"スプライトの列数", &m_constants.columns);
    ImGui::InputInt((char*)u8"スプライトの行数", &m_constants.rows);
    ImGui::Checkbox((char*)u8"発光フラグ", reinterpret_cast<bool*>(&m_constants.onflag));
}
