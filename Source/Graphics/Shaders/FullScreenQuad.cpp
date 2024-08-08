#include "FullScreenQuad.h"
#include "Graphics/Graphics.h"
#include "Shader.h"
#include "Misc.h"
#include "Texture.h"

//コンストラクタ
FullScreenQuad::FullScreenQuad()
{
    ID3D11Device* device = Graphics::Instance().GetDevice();

    //頂点シェーダー
    CreateVsFromCso(device, "Shader\\FullScreenQuadVS.cso", m_embeddedvertexshader.ReleaseAndGetAddressOf(), nullptr, nullptr, 0);

    // ピクセルシェーダー
    CreatePsFromCso(device, "Shader\\FullScreenQuadPS.cso", m_embeddedpixelshader.ReleaseAndGetAddressOf());
}

//元々のピクセルシェーダを参照してキャッシュする関数(フルスクリーンの四角形を描画して、指定されたピクセルシェーダやリソースビューを設定し描画をした後もとに戻している)
void FullScreenQuad::Blit(ID3D11DeviceContext* immediate_context, ID3D11ShaderResourceView* const* shader_resource_views, uint32_t start_slot, uint32_t num_views, ID3D11PixelShader* replaced_pixel_shader)
{
    // 現在のピクセルシェーダーのリソースビューをキャッシュ
    immediate_context->PSGetShaderResources(start_slot, num_views, m_cachedShaderResourceViews);

    // 新しいピクセルシェーダーのリソースビューをセット
    immediate_context->PSSetShaderResources(start_slot, num_views, shader_resource_views);

    // 頂点バッファ, プリミティブのトポロジー, 入力レイアウトを設定
    immediate_context->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
    immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    immediate_context->IASetInputLayout(NULL);

    // 埋め込まれた頂点シェーダーを設定
    immediate_context->VSSetShader(m_embeddedvertexshader.Get(), 0, 0);
    replaced_pixel_shader ? immediate_context->PSSetShader(replaced_pixel_shader, 0, 0) : immediate_context->PSSetShader(m_embeddedpixelshader.Get(), 0, 0);

    // ピクセルシェーダーのリソースビューを再度設定
    immediate_context->PSSetShaderResources(start_slot, num_views, shader_resource_views);

    immediate_context->RSSetState(Graphics::Instance().GetRasterizerState(RASTERIZERSTATE::SOLID_CULL_NONE));

    // 4つの頂点からなる三角形ストリップ
    immediate_context->Draw(4, 0);

    // 元のピクセルシェーダーのリソースビューを復元
    immediate_context->PSSetShaderResources(start_slot, num_views, m_cachedShaderResourceViews);
}