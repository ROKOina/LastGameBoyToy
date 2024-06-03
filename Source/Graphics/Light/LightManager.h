#pragma once

#include <vector>
#include <memory>
#include <wrl.h>
#include <d3d11.h>
#include "Light.h"

// ライト管理クラス
class LightManager
{
    LightManager();
    ~LightManager();

public:

    //インスタンス取得
    static LightManager& Instance()
    {
        static	LightManager	lightManager;
        return	lightManager;
    }

    // ライトを登録する
    void Register(Light* light);

    // ライトの登録を解除する
    void Remove(Light* light);

    // 登録済みのライトを全削除する
    void Clear();

    // ライト情報をRenderContextに積む
    void UpdateConstatBuffer();

    // デバッグ情報の表示
    void DrawDebugGUI();

    // デバッグプリミティブの表示
    void DrawDebugPrimitive();

    // ライトの情報
    int GetLightCount() { return static_cast<int>(m_lights.size()); }
    Light* GetLight(const int& index) const { return m_lights.at(index); }

private:
    std::vector<Light*>	m_lights;

    Microsoft::WRL::ComPtr<ID3D11Buffer>m_lightCb; // 定数バッファ
};
