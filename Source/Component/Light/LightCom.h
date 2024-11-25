#pragma once

#include "Component\System\Component.h"
#include <d3d11.h>
#include <wrl.h>

class Light :public Component
{
public:

    Light(const char* filename);
    ~Light() {};

    //初期設定
    void Start()override;

    //更新処理
    void Update(float elapsedTime)override;

    //imgui
    void OnGUI()override;

    //名前設定
    const char* GetName() const override { return "Light"; }

private:

    //デバッグ描画
    void DebugPrimitive();

    //シリアライズ
    void Serialize();

    //デシリアライズ
    void Desirialize(const char* filename);

    //読み込み
    void LoadDesirialize();

public:

    // ライトの向き
    DirectX::XMFLOAT3 GetDirection() const { return { cb.directionalLight.direction.x, cb.directionalLight.direction.y, cb.directionalLight.direction.z }; }

public:

    // 光源タイプ
    enum class LightType
    {
        Directional,	// 平行光源
        Point,			// 点光源
        Spot,			// スポットライト
        MAX
    };

    //平行光源
    struct DirectionalLightData
    {
        DirectX::XMFLOAT4	direction = { 0.0f,-1.0f,-1.0f,0.0f };
        DirectX::XMFLOAT4	color = { 1,1,1,1 };

        template<class Archive>
        void serialize(Archive& archive, int version);
    };

    // 点光源情報
    struct PointLightData
    {
        DirectX::XMFLOAT4	position = {};
        DirectX::XMFLOAT4	color = { 1,1,1,1 };
        float			    range = { 1.0f };
        DirectX::XMFLOAT3	dummy = {};

        template<class Archive>
        void serialize(Archive& archive, int version);
    };
    // 点光源の最大数
    static	constexpr	int	POINT_LIGHT_MAX = 10;

    // スポットライト情報
    struct SpotLightData
    {
        DirectX::XMFLOAT4	position = {};
        DirectX::XMFLOAT4	direction = {};
        DirectX::XMFLOAT4	color = { 1,1,1,1 };
        float			    range = { 1.0f };
        float			    innerCorn = {}; 	// インナー角度範囲
        float			    outerCorn = {}; 	// アウター角度範囲
        float			    dummy = {};

        template<class Archive>
        void serialize(Archive& archive, int version);
    };
    // スポットライトの最大数
    static	constexpr	int	SPOT_LIGHT_MAX = 10;

    // 定数バッファ用構造体
    struct LightCB
    {
        DirectionalLightData directionalLight = {};
        PointLightData       pointLight[POINT_LIGHT_MAX];
        SpotLightData        spotLight[SPOT_LIGHT_MAX];

        template<class Archive>
        void serialize(Archive& archive, int version);
    };
    LightCB cb = {};

    //ライトのパラメータ
    struct LightParameter
    {
        float power = 1.0f;        //ライトの光の強さ
        int lighttype = 0;         //ライトタイプ

        template<class Archive>
        void serialize(Archive& archive, int version);
    };
    LightParameter LP = {};

private:
    Microsoft::WRL::ComPtr<ID3D11Buffer>m_lightCb;       // 定数バッファ
    LightType m_lightType = LightType::Directional;		 // ライトタイプ
    DirectX::XMFLOAT4 directioncolor = { 1,1,1,1 };
    int pointLightCount = 0;
    int spotlightmax = 0;
};