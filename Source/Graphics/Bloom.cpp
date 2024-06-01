#include "Shader.h"
#include "Bloom.h"
#include "Graphics.h"
#include <vector>

Bloom::Bloom(ID3D11Device* device, uint32_t width, uint32_t height) : FullScreenQuad(device)
{
    glow_extraction = std::make_unique<FrameBuffer>(device, width, height, DXGI_FORMAT_R32G32B32A32_FLOAT, true);
    for (size_t downsampled_index = 0; downsampled_index < downsampled_count; ++downsampled_index)
    {
        gaussian_blur[downsampled_index][0] = std::make_unique<FrameBuffer>(device, width >> downsampled_index, height >> downsampled_index, DXGI_FORMAT_R32G32B32A32_FLOAT, true);
        gaussian_blur[downsampled_index][1] = std::make_unique<FrameBuffer>(device, width >> downsampled_index, height >> downsampled_index, DXGI_FORMAT_R32G32B32A32_FLOAT, true);
    }

    //ピクセルシェーダセット
    create_ps_from_cso(device, "Shader\\GlowExtractionPS.cso", glow_extraction_ps.GetAddressOf());
    create_ps_from_cso(device, "Shader\\GaussianBlurVerticalPS.cso", gaussian_blur_vertical_ps.GetAddressOf());
    create_ps_from_cso(device, "Shader\\GaussianBlurUpsamplingPS.cso", gaussian_blur_upsampling_ps.GetAddressOf());
    create_ps_from_cso(device, "Shader\\GaussianBlurHorizontalPS.cso", gaussian_blur_horizontal_ps.GetAddressOf());
    create_ps_from_cso(device, "Shader\\GaussianBlurDownsamplingPS.cso", gaussian_blur_downsampling_ps.GetAddressOf());
}

void Bloom::make(ID3D11DeviceContext* immediate_context, ID3D11ShaderResourceView* color_map)
{
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> null_shader_resource_view;

    //Extracting bright color
    glow_extraction->clear(immediate_context);
    glow_extraction->activate(immediate_context);
    FullScreenQuad::blit(immediate_context, &color_map, 0, 1, glow_extraction_ps.Get());
    glow_extraction->deactivate(immediate_context);

    //Gaussian blur
    //Efficient Gaussian blur with linear sampling
    //http://rastergrid.com/blog/2010/09/efficient-gaussian-blur-with-linear-sampling/
    // downsampling
    gaussian_blur[0][0]->clear(immediate_context);
    gaussian_blur[0][0]->activate(immediate_context);
    FullScreenQuad::blit(immediate_context, glow_extraction->shader_resource_views[0].GetAddressOf(), 0, 1, gaussian_blur_downsampling_ps.Get());
    gaussian_blur[0][0]->deactivate(immediate_context);

    // ping-pong gaussian blur
    gaussian_blur[0][1]->clear(immediate_context);
    gaussian_blur[0][1]->activate(immediate_context);
    FullScreenQuad::blit(immediate_context, gaussian_blur[0][0]->shader_resource_views[0].GetAddressOf(), 0, 1, gaussian_blur_horizontal_ps.Get());
    gaussian_blur[0][1]->deactivate(immediate_context);

    gaussian_blur[0][0]->clear(immediate_context);
    gaussian_blur[0][0]->activate(immediate_context);
    FullScreenQuad::blit(immediate_context, gaussian_blur[0][1]->shader_resource_views[0].GetAddressOf(), 0, 1, gaussian_blur_vertical_ps.Get());
    gaussian_blur[0][0]->deactivate(immediate_context);

    for (size_t downsampled_index = 1; downsampled_index < downsampled_count; ++downsampled_index)
    {
        // downsampling
        gaussian_blur[downsampled_index][0]->clear(immediate_context);
        gaussian_blur[downsampled_index][0]->activate(immediate_context);
        FullScreenQuad::blit(immediate_context, gaussian_blur[downsampled_index - 1][0]->shader_resource_views[0].GetAddressOf(), 0, 1, gaussian_blur_downsampling_ps.Get());
        gaussian_blur[downsampled_index][0]->deactivate(immediate_context);

        // ping-pong gaussian blur
        gaussian_blur[downsampled_index][1]->clear(immediate_context);
        gaussian_blur[downsampled_index][1]->activate(immediate_context);
        FullScreenQuad::blit(immediate_context, gaussian_blur[downsampled_index][0]->shader_resource_views[0].GetAddressOf(), 0, 1, gaussian_blur_horizontal_ps.Get());
        gaussian_blur[downsampled_index][1]->deactivate(immediate_context);

        gaussian_blur[downsampled_index][0]->clear(immediate_context);
        gaussian_blur[downsampled_index][0]->activate(immediate_context);
        FullScreenQuad::blit(immediate_context, gaussian_blur[downsampled_index][1]->shader_resource_views[0].GetAddressOf(), 0, 1, gaussian_blur_vertical_ps.Get());
        gaussian_blur[downsampled_index][0]->deactivate(immediate_context);
    }
}

void Bloom::blit(ID3D11DeviceContext* immediate_context)
{
    std::vector<ID3D11ShaderResourceView*> shader_resource_views;
    for (size_t downsampled_index = 0; downsampled_index < downsampled_count; ++downsampled_index)
    {
        shader_resource_views.push_back(gaussian_blur[downsampled_index][0]->shader_resource_views[0].Get());
    }
    FullScreenQuad::blit(immediate_context, shader_resource_views.data(), 0, downsampled_count, gaussian_blur_upsampling_ps.Get());
}