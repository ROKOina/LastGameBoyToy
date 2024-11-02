#pragma once

#include <array>
#include <d3d11.h>
#include <wrl.h>

class SkyBoxManager
{
private:
    SkyBoxManager();
    SkyBoxManager(SkyBoxManager&) {}

public:
    // シングルトン
    static SkyBoxManager& Instance() {
        static SkyBoxManager instance;
        return instance;
    }

    // 引数：スカイボックスに関係する4つのテクスチャのパス
    void LoadSkyBoxTextures(std::array<const char*, 4> filepath);

    // SRVの設定 引数：SRV登録番号
    void BindTextures(ID3D11DeviceContext* dc, int startRegisterIndex);

    // スカイボックスの描画
    void DrawSkyBox(ID3D11DeviceContext* dc);

private:
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> skyBoxTextures[4];
    Microsoft::WRL::ComPtr<ID3D11PixelShader> skyBoxShader;
};
