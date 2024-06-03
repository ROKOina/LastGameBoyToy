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

    void Make(ID3D11DeviceContext* immediate_context, ID3D11ShaderResourceView* color_map);
    void Blit(ID3D11DeviceContext* immediate_context);

private:
    std::unique_ptr<FrameBuffer> m_glowextraction;

    static const size_t m_downsampledcount = 6;
    std::unique_ptr<FrameBuffer> m_gaussianblur[m_downsampledcount][2];

private:
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_glowextractionps;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_gaussianblurhorizontalps;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_gaussianblurverticalps;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_gaussianblurupsamplingps;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_gaussianblurdownsamplingps;
};