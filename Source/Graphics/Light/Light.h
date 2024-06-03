#pragma once
#include "LightData.h"

// 光源タイプ
enum class LightType
{
    Directional,	// 平行光源
    Point,			// 点光源
    Spot,			// スポットライト
};

// 光源クラス
class Light
{
public:
    Light(LightType lightType = LightType::Directional);

    // ライト情報をRenderContextに積む
    void PushRenderContext(LightCB& cb);

    // デバッグ情報の表示
    void DrawDebugGUI();

    // デバッグ図形の表示
    void DrawDebugPrimitive();

    // ライトタイプ
    const LightType& GetLightType()const { return m_lightType; }

    // ライトの座標
    const DirectX::XMFLOAT3& Position() const { return m_position; }
    void SetPosition(DirectX::XMFLOAT3 position) { this->m_position = position; }

    // ライトの向き
    const DirectX::XMFLOAT3& GetDirection() const { return m_direction; }
    void SetDirection(DirectX::XMFLOAT3 direction) { this->m_direction = direction; }

    // 色
    const DirectX::XMFLOAT4& GetColor() const { return m_color; }
    void SetColor(DirectX::XMFLOAT4 color) { this->m_color = color; }

    void SetPower(float power) { this->m_power = power; }

    // ライトの範囲
    const float& GetRange() const { return m_range; }
    void SetRange(float range) { this->m_range = range; }

    // インナー
    const float& GetInnerCorn()const { return m_innercorn; }
    void SetInnerCorn(float innerCorn) { this->m_innercorn = innerCorn; }

    // アウター
    const float& GetOuterCorn()const { return m_outercorn; }
    void SetOuterCorn(float outerCorn) { this->m_outercorn = outerCorn; }

private:
    LightType			m_lightType = LightType::Directional;		 // ライトタイプ
    DirectX::XMFLOAT3	m_position = DirectX::XMFLOAT3(0, 0, 0);	 // ライトの座標
    DirectX::XMFLOAT3	m_direction = DirectX::XMFLOAT3(0, -1, -1);// ライトの向き
    DirectX::XMFLOAT4	m_color = DirectX::XMFLOAT4(1, 1, 1, 1);	 // ライトの色
    float	m_range = 20.0f;	// 範囲
    float	m_power = 1.0f;		// ライトの強さ

    float	m_innercorn = 0.99f;	// インナー
    float	m_outercorn = 0.9f;	// アウター
};