#pragma once

#include "GameSource/Math/Mathf.h"
#include "Graphics/Shaders/ConstantBuffer.h"
#include <memory>

class BaseConstants {
public:
  virtual void DrawGui() = 0;

  virtual void Update(const float& elapsedTime) = 0;

  virtual void UpdateConstantBuffer(ID3D11DeviceContext* dc) = 0;

};

class EffectConstants :public BaseConstants
{
public:
  EffectConstants();

  void DrawGui()override;

  void Update(const float& elapsedTime)override;

  void UpdateConstantBuffer(ID3D11DeviceContext* dc)override;

public:
  struct Buffer {
    float simulateTime1 = 0.0f;
    float simulateTime2 = 0.0f;
    float simulateTime3 = 0.0f;
    float waveEffectRange = 3.0f;

    DirectX::XMFLOAT4 waveEffectColor = { 1.0f,1.0f,1.0f,1.0f };
    
    float waveEffectIntensity = 1.0f;
    DirectX::XMFLOAT3 dummy;
  };

public:
  float simulateSpeed1 = 1.0f;
  float simulateSpeed2 = 1.0f;
  float simulateSpeed3 = 1.0f;

  float waveEffectRange = 3.0f;
  DirectX::XMFLOAT4 waveEffectColor = { 1.0f,1.0f,1.0f,1.0f };
  float waveEffectIntensity = 1.0f;

private:
  inline static std::unique_ptr<ConstantBuffer<Buffer>> m_effectconstants;

};
