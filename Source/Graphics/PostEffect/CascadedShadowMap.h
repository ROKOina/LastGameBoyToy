#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <directxmath.h>
#include <vector>
#include <functional>
#include <memory>
#include "Graphics/ConstantBuffer.h"

// https://learnopengl.com/Guest-Articles/2021/CSM
// https://learn.microsoft.com/en-us/windows/win32/dxtecharts/cascaded-shadow-maps
// https://developer.nvidia.com/gpugems/gpugems3/part-ii-light-and-shadows/chapter-10-parallel-split-shadow-maps-programmable-gpus
class CascadedShadowMap
{
public:
    CascadedShadowMap(ID3D11Device* device, uint32_t width, uint32_t height);
    virtual ~CascadedShadowMap() = default;
    CascadedShadowMap(const CascadedShadowMap&) = delete;
    CascadedShadowMap& operator =(const CascadedShadowMap&) = delete;
    CascadedShadowMap(CascadedShadowMap&&) noexcept = delete;
    CascadedShadowMap& operator =(CascadedShadowMap&&) noexcept = delete;

    //é¿çs
    void Make(ID3D11DeviceContext* immediate_context,
        const DirectX::XMFLOAT4X4& camera_view,
        const DirectX::XMFLOAT4X4& camera_projection,
        const DirectX::XMFLOAT3& light_direction,
        float critical_depth_value, // If this value is 0, the camera's far panel distance is used.
        std::function<void()> drawcallback);

    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_depthstencilbuffer;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthstencilview;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderresourceview;
    D3D11_VIEWPORT m_viewport;

    std::vector<DirectX::XMFLOAT4X4> m_viewprojection;
    std::vector<float> m_distances;

    struct constants
    {
        DirectX::XMFLOAT4X4 viewprojectionmatrices[4];
        float cascadeplanedistances[4];
    };
    std::unique_ptr<ConstantBuffer<constants>> m_constants;

public:
    const size_t m_cascadecount;
    float m_splitschemeweight = 0.82f; // logarithmic_split_scheme * _split_scheme_weight + uniform_split_scheme * (1 - _split_scheme_weight)
};