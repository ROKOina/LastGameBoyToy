#include "FullScreenQuad.h"
#include "Graphics.h"
#include "Shader.h"
#include "System/Misc.h"
#include "Texture.h"

static FullScreenQuad* instance = nullptr;

FullScreenQuad::FullScreenQuad(ID3D11Device* device)
{
    //インスタンスポインタ設定
    instance = this;

    //頂点シェーダー
    create_vs_from_cso(device, "Shader\\FullScreenQuadVS.cso", embedded_vertex_shader.ReleaseAndGetAddressOf(), nullptr, nullptr, 0);

    // ピクセルシェーダー
    create_ps_from_cso(device, "Shader\\FullScreenQuadPS.cso", embedded_pixel_shader.ReleaseAndGetAddressOf());

    //スカイマップ用ピクセルシェーダー
    create_ps_from_cso(device, "Shader\\SkyBoxPS.cso", skymappixelshader.ReleaseAndGetAddressOf());

    //SKYMAP用テクスチャを読み込み
    D3D11_TEXTURE2D_DESC texture2d_desc{};
   // load_texture_from_file(device, L".\\Data\\Texture\\snowy_hillside_4k.DDS", skymap.GetAddressOf(), &texture2d_desc);
}

//インスタンス取得
FullScreenQuad& FullScreenQuad::Instance()
{
    return *instance;
}

//元々のピクセルシェーダを参照してキャッシュする関数(フルスクリーンの四角形を描画して、指定されたピクセルシェーダやリソースビューを設定し描画をした後もとに戻している)
void FullScreenQuad::blit(ID3D11DeviceContext* immediate_context, ID3D11ShaderResourceView* const* shader_resource_views, uint32_t start_slot, uint32_t num_views, ID3D11PixelShader* replaced_pixel_shader)
{
    //現在のピクセルシェーダーのリソースビューをキャッシュ
    ID3D11ShaderResourceView* cached_shader_resource_views[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT]{};
    immediate_context->PSGetShaderResources(start_slot, num_views, cached_shader_resource_views);

    //新しいピクセルシェーダーのリソースビューをセット
    immediate_context->PSSetShaderResources(start_slot, num_views, shader_resource_views);

    //頂点バッファ,プリミティブのトポロジー,入力レイアウトを設定
    immediate_context->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
    immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    immediate_context->IASetInputLayout(NULL);

    //埋め込まれた頂点シェーダーを設定
    immediate_context->VSSetShader(embedded_vertex_shader.Get(), 0, 0);
    replaced_pixel_shader ? immediate_context->PSSetShader(replaced_pixel_shader, 0, 0) : immediate_context->PSSetShader(embedded_pixel_shader.Get(), 0, 0);

    //ピクセルシェーダーのリソースビューを再度設定
    immediate_context->PSSetShaderResources(start_slot, num_views, shader_resource_views);

    //4つの頂点からなる三角形ストリップ
    immediate_context->Draw(4, 0);

    //元のピクセルシェーダーのリソースビューを復元
    immediate_context->PSSetShaderResources(start_slot, num_views, cached_shader_resource_views);
    for (ID3D11ShaderResourceView* cached_shader_resource_view : cached_shader_resource_views)
    {
        if (cached_shader_resource_view) cached_shader_resource_view->Release();
    }
}

//skymap
void FullScreenQuad::SkyMap()
{
    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();
    dc->OMSetDepthStencilState(Graphics::Instance().GetDepthStencilState(DEPTHSTATE::ZT_OFF_ZW_OFF), 0);
    dc->RSSetState(Graphics::Instance().GetRasterizerState(RASTERIZERSTATE::SOLID_CULL_NONE));
    blit(graphics.GetDeviceContext(), skymap.GetAddressOf(), 10, 1, skymappixelshader.Get());
}