#pragma once

#include "FrameBuffer.h"
#include "FullScreenQuad.h"
#include <memory>

//ÉuÉãÅ[ÉÄ
class Bloom :FullScreenQuad
{
public:
    Bloom(ID3D11Device* device, uint32_t width, uint32_t height);
    ~Bloom() = default;
    Bloom(const Bloom&) = delete;
    Bloom& operator =(const Bloom&) = delete;
    Bloom(Bloom&&) noexcept = delete;
    Bloom& operator =(Bloom&&) noexcept = delete;

    void make(ID3D11DeviceContext* immediate_context, ID3D11ShaderResourceView* color_map);
    void blit(ID3D11DeviceContext* immediate_context);

private:
    std::unique_ptr<FrameBuffer> glow_extraction;

    static const size_t downsampled_count = 6;
    std::unique_ptr<FrameBuffer> gaussian_blur[downsampled_count][2];

private:
    Microsoft::WRL::ComPtr<ID3D11PixelShader> glow_extraction_ps;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> gaussian_blur_horizontal_ps;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> gaussian_blur_vertical_ps;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> gaussian_blur_upsampling_ps;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> gaussian_blur_downsampling_ps;
};