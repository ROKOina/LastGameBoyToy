#include "Texture.h"

//DDSTexture読み込み
static std::map<std::wstring, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> resources;
HRESULT LoadTextureFromFile(ID3D11Device* device, const char* filename, ID3D11ShaderResourceView** shader_resource_view, D3D11_TEXTURE2D_DESC* texture2d_desc)
{
    HRESULT hr{ S_OK };
    Microsoft::WRL::ComPtr<ID3D11Resource> resource;

    std::wstring wideFilename(filename, filename + strlen(filename));

    auto it = resources.find(wideFilename);
    if (it != resources.end())
    {
        *shader_resource_view = it->second.Get();
        (*shader_resource_view)->AddRef();
        (*shader_resource_view)->GetResource(resource.GetAddressOf());
    }
    else
    {
        std::filesystem::path filepath(wideFilename);

        // DDSファイルの存在を確認
        std::filesystem::path dds_filepath = filepath;
        dds_filepath.replace_extension("dds");

        if (std::filesystem::exists(dds_filepath))
        {
            // DDSテクスチャの読み込み
            hr = DirectX::CreateDDSTextureFromFile(device, dds_filepath.c_str(), resource.GetAddressOf(), shader_resource_view);
            _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
        }
        else
        {
            // HDRファイルの存在を確認
            std::filesystem::path hdr_filepath = filepath;
            hdr_filepath.replace_extension("hdr");

            if (std::filesystem::exists(hdr_filepath))
            {
                // HDRテクスチャの読み込み
                hr = LoadHDRTexture(device, hdr_filepath.c_str(), resource, shader_resource_view);
                _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
            }
            else
            {
                // その他のWIC対応フォーマットの読み込み
                hr = DirectX::CreateWICTextureFromFile(device, filepath.c_str(), resource.GetAddressOf(), shader_resource_view);
                _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
            }
        }

        resources.insert(make_pair(wideFilename, *shader_resource_view));
    }

    if (texture2d_desc)
    {
        Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
        hr = resource.Get()->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
        texture2d->GetDesc(texture2d_desc);
    }

    return hr;
}

HRESULT LoadHDRTexture(ID3D11Device* device, const std::wstring& filename, Microsoft::WRL::ComPtr<ID3D11Resource>& resource, ID3D11ShaderResourceView** shader_resource_view)
{
    int width, height, channels;
    stbi_set_flip_vertically_on_load(false); // HDRファイルの読み込み時に上下反転を設定

    // stb_imageでHDRファイルを読み込む
    float* hdrData = stbi_loadf(std::string(filename.begin(), filename.end()).c_str(), &width, &height, &channels, 4);
    if (!hdrData)
    {
        return E_FAIL; // HDRファイル読み込み失敗
    }

    // テクスチャの説明を作成
    D3D11_TEXTURE2D_DESC texDesc = {};
    texDesc.Width = width;
    texDesc.Height = height;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; // HDRは浮動小数点フォーマット
    texDesc.SampleDesc.Count = 1;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    // 初期化データ
    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = hdrData;
    initData.SysMemPitch = width * 4 * sizeof(float);

    // テクスチャ作成
    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
    HRESULT hr = device->CreateTexture2D(&texDesc, &initData, texture.GetAddressOf());
    stbi_image_free(hdrData); // 読み込みデータの解放
    if (FAILED(hr))
    {
        return hr; // テクスチャ作成失敗
    }

    // シェーダーリソースビュー作成
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = texDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = texDesc.MipLevels;

    hr = device->CreateShaderResourceView(texture.Get(), &srvDesc, shader_resource_view);
    if (FAILED(hr))
    {
        return hr;
    }

    resource = texture; // リソースを保持
    return S_OK;
}

//解放処理
void ReleaseAllTextures()
{
    resources.clear();
}

HRESULT LoadTexture(const char* filename, ID3D11Device* device, ID3D11ShaderResourceView** srv, D3D11_TEXTURE2D_DESC* textureDesc)
{
    // マルチバイト文字からワイド文字へ変換
    std::filesystem::path filepath(filename);
    Microsoft::WRL::ComPtr<ID3D11Resource> resource;
    HRESULT hr;

    // DDSの読み込み
    if (filepath.extension().string() == ".DDS" ||
        filepath.extension().string() == ".dds")
    {
        hr = DirectX::CreateDDSTextureFromFile(device, filepath.c_str(), resource.GetAddressOf(), srv);
    }
    // DDS以外のテクスチャ読み込み
    else
    {
        hr = DirectX::CreateWICTextureFromFile(device, filepath.c_str(), resource.GetAddressOf(), srv);
    }
    // D3D11_TEXTURE2D_DESCの作成
    if (textureDesc && SUCCEEDED(hr))
    {
        Microsoft::WRL::ComPtr<ID3D11Texture2D>	texture2d;
        HRESULT hr = resource.Get()->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));
        texture2d->GetDesc(textureDesc);
    }

    return hr;
}