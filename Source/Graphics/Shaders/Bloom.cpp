#include "Shader.h"
#include "Bloom.h"
#include "Graphics/Graphics.h"
#include <vector>

//コンストラクタ
Bloom::Bloom(ID3D11Device* device, uint32_t width, uint32_t height)
{
    m_glowextraction = std::make_unique<FrameBuffer>(device, width, height, DXGI_FORMAT_R32G32B32A32_FLOAT, true);
    for (size_t downsampled_index = 0; downsampled_index < m_downsampledcount; ++downsampled_index)
    {
        m_gaussianblur[downsampled_index][0] = std::make_unique<FrameBuffer>(device, width >> downsampled_index, height >> downsampled_index, DXGI_FORMAT_R32G32B32A32_FLOAT, true);
        m_gaussianblur[downsampled_index][1] = std::make_unique<FrameBuffer>(device, width >> downsampled_index, height >> downsampled_index, DXGI_FORMAT_R32G32B32A32_FLOAT, true);
    }

    //ピクセルシェーダセット
    CreatePsFromCso(device, "Shader\\GlowExtractionPS.cso", m_glowextractionps.GetAddressOf());
    CreatePsFromCso(device, "Shader\\GaussianBlurVerticalPS.cso", m_gaussianblurverticalps.GetAddressOf());
    CreatePsFromCso(device, "Shader\\GaussianBlurUpsamplingPS.cso", m_gaussianblurupsamplingps.GetAddressOf());
    CreatePsFromCso(device, "Shader\\GaussianBlurHorizontalPS.cso", m_gaussianblurhorizontalps.GetAddressOf());
    CreatePsFromCso(device, "Shader\\GaussianBlurDownsamplingPS.cso", m_gaussianblurdownsamplingps.GetAddressOf());
}

//縮小バッファ作成
void Bloom::Make(ID3D11DeviceContext* immediate_context, ID3D11ShaderResourceView* color_map)
{
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> null_shader_resource_view;
    FullScreenQuad& screenQuad = FullScreenQuad::Instance();

    //Extracting bright color
    m_glowextraction->Clear(immediate_context);
    m_glowextraction->Activate(immediate_context);
    screenQuad.Blit(immediate_context, &color_map, 0, 1, m_glowextractionps.Get());
    m_glowextraction->Deactivate(immediate_context);

    //Gaussian blur
    //Efficient Gaussian blur with linear sampling
    //http://rastergrid.com/blog/2010/09/efficient-gaussian-blur-with-linear-sampling/
    // downsampling
    m_gaussianblur[0][0]->Clear(immediate_context);
    m_gaussianblur[0][0]->Activate(immediate_context);
    screenQuad.Blit(immediate_context, m_glowextraction->m_shaderresourceviews[0].GetAddressOf(), 0, 1, m_gaussianblurdownsamplingps.Get());
    m_gaussianblur[0][0]->Deactivate(immediate_context);

    // ping-pong gaussian blur
    m_gaussianblur[0][1]->Clear(immediate_context);
    m_gaussianblur[0][1]->Activate(immediate_context);
    screenQuad.Blit(immediate_context, m_gaussianblur[0][0]->m_shaderresourceviews[0].GetAddressOf(), 0, 1, m_gaussianblurhorizontalps.Get());
    m_gaussianblur[0][1]->Deactivate(immediate_context);

    m_gaussianblur[0][0]->Clear(immediate_context);
    m_gaussianblur[0][0]->Activate(immediate_context);
    screenQuad.Blit(immediate_context, m_gaussianblur[0][1]->m_shaderresourceviews[0].GetAddressOf(), 0, 1, m_gaussianblurverticalps.Get());
    m_gaussianblur[0][0]->Deactivate(immediate_context);

    for (size_t downsampled_index = 1; downsampled_index < m_downsampledcount; ++downsampled_index)
    {
        // downsampling
        m_gaussianblur[downsampled_index][0]->Clear(immediate_context);
        m_gaussianblur[downsampled_index][0]->Activate(immediate_context);
        screenQuad.Blit(immediate_context, m_gaussianblur[downsampled_index - 1][0]->m_shaderresourceviews[0].GetAddressOf(), 0, 1, m_gaussianblurdownsamplingps.Get());
        m_gaussianblur[downsampled_index][0]->Deactivate(immediate_context);

        // ping-pong gaussian blur
        m_gaussianblur[downsampled_index][1]->Clear(immediate_context);
        m_gaussianblur[downsampled_index][1]->Activate(immediate_context);
        screenQuad.Blit(immediate_context, m_gaussianblur[downsampled_index][0]->m_shaderresourceviews[0].GetAddressOf(), 0, 1, m_gaussianblurhorizontalps.Get());
        m_gaussianblur[downsampled_index][1]->Deactivate(immediate_context);

        m_gaussianblur[downsampled_index][0]->Clear(immediate_context);
        m_gaussianblur[downsampled_index][0]->Activate(immediate_context);
        screenQuad.Blit(immediate_context, m_gaussianblur[downsampled_index][1]->m_shaderresourceviews[0].GetAddressOf(), 0, 1, m_gaussianblurverticalps.Get());
        m_gaussianblur[downsampled_index][0]->Deactivate(immediate_context);
    }
}

//ここで実装する
void Bloom::Blit(ID3D11DeviceContext* immediate_context)
{
    std::vector<ID3D11ShaderResourceView*> shader_resource_views;
    for (size_t downsampled_index = 0; downsampled_index < m_downsampledcount; ++downsampled_index)
    {
        shader_resource_views.push_back(m_gaussianblur[downsampled_index][0]->m_shaderresourceviews[0].Get());
    }
    FullScreenQuad::Instance().Blit(immediate_context, shader_resource_views.data(), 0, m_downsampledcount, m_gaussianblurupsamplingps.Get());
}