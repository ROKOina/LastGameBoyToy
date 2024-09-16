#include "BulletHoleCom.h"
#include "Graphics/Shaders/FullScreenQuad.h"
#include "Graphics/Shaders/Shader.h"
#include "Graphics/Graphics.h"
#include "Graphics/Shaders/Texture.h"
#include "Graphics/Shaders/PostEffect.h"

//初期化
BulletHole::BulletHole(const char* filename)
{
    Graphics& Graphics = Graphics::Instance();

    //ピクセルシェーダー読み込み
    CreatePsFromCso(Graphics.GetDevice(), "Shader\\BulletHolePS.cso", m_pixelshaders.GetAddressOf());

    //テクスチャ読み込み
    D3D11_TEXTURE2D_DESC texture2d_desc{};
    LoadTextureFromFile(Graphics.GetDevice(), filename, m_shaderresourceview.ReleaseAndGetAddressOf(), &texture2d_desc);

    //コンスタントバッファ
    m_special = std::make_unique<ConstantBuffer<SPECIALFFECT>>(Graphics.GetDevice());
}

//更新処理
void BulletHole::Update(float elapsedTime)
{
    Graphics& Graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = Graphics.GetDeviceContext();

    m_special->Activate(dc, (int)CB_INDEX::PROJECTIONMAPPING, false, true, false, false, false, false);
    DirectX::XMStoreFloat4x4(&m_special->data.projectionmappingtransform,
        DirectX::XMMatrixLookAtLH(
            DirectX::XMLoadFloat3(&eye),
            DirectX::XMLoadFloat3(&focus),
            DirectX::XMVector3Transform(DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), DirectX::XMMatrixRotationRollPitchYaw(0, DirectX::XMConvertToRadians(0), 0))) *
        DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(fovy), 1.0f, 1.0f, 500.0f)
    );
}

//描画処理
void BulletHole::Render()
{
    Graphics& Graphics = Graphics::Instance();
    PostEffect& pf = PostEffect::Instance();
    ID3D11DeviceContext* dc = Graphics.GetDeviceContext();

    dc->OMSetBlendState(Graphics.GetBlendState(BLENDSTATE::NONE), nullptr, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(Graphics.GetDepthStencilState(DEPTHSTATE::ZT_OFF_ZW_OFF), 1);
    dc->RSSetState(Graphics.GetRasterizerState(RASTERIZERSTATE::SOLID_CULL_NONE));
    ID3D11ShaderResourceView* r[]
    { m_shaderresourceview.Get(),pf.GetMultiRenderTarget()->GetShaderResources()[2],pf.GetMultiRenderTarget()->GetShaderResources()[6],pf.m_offScreenBuffer[static_cast<size_t>(PostEffect::offscreen::posteffect)]->m_shaderresourceviews[0].Get() };
    FullScreenQuad::Instance().Blit(dc, r, 0, _countof(r), m_pixelshaders.Get());
    pf.m_offScreenBuffer[static_cast<size_t>(PostEffect::offscreen::specialeffect)]->Deactivate(dc);
}

//gui
void BulletHole::OnGUI()
{
    ImGui::DragFloat3("eye", &eye.x);
    ImGui::DragFloat3("focus", &focus.x);
    ImGui::SliderFloat("fovy", &fovy, 10.0f, 180.0f);
}