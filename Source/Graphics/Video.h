#pragma once

#include <d3d11.h>
#include <stdint.h>
#include <wrl.h>
#include <memory>

//テクスチャ
struct Texture
{
    ID3D11Resource* texture = nullptr;
    ID3D11ShaderResourceView* shader_resource_view = nullptr;
    DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
    uint32_t xres = 0;
    uint32_t yres = 0;
    bool create(uint32_t xres, uint32_t yres, DXGI_FORMAT new_format, bool is_dynamic);
    void destroy();
    void activate(int slot) const;
    bool updateFromIYUV(const uint8_t* new_data, size_t data_size);
};

//ビデオの設定
struct VideoTexture
{
    static bool createAPI();
    static void destroyAPI();

    struct InternalData;
    InternalData* internal_data = nullptr;

    bool create(const char* filename);
    void destroy();
    bool update(float dt);

    void pause();
    void resume();
    bool hasFinished();
    Texture* getTexture();
    float getAspectRatio() const;
};