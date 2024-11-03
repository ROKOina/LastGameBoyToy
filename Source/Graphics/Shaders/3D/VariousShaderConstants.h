#pragma once

#include "GameSource/Math/Mathf.h"
#include "Graphics/Shaders/ConstantBuffer.h"
#include <memory>

class BaseConstants {
public:
  virtual void DrawGui() = 0;

  virtual void Update(const float& elapsedTime) {};

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

class SciFiGateConstants :public BaseConstants
{
public:
  SciFiGateConstants();

  void DrawGui()override;

  void Update(const float& elapsedTime)override;

  void UpdateConstantBuffer(ID3D11DeviceContext* dc)override;

public:
  struct Buffer {
    DirectX::XMFLOAT2 uvScrollDir1 = {};
    DirectX::XMFLOAT2 uvScrollDir2 = {};

    DirectX::XMFLOAT2 uvScale1 = {};
    DirectX::XMFLOAT2 uvScale2 = {};
    float simulateTime1 = 0.0f;
    float simulateTime2 = 0.0f;
    float intensity1 = 0.0f;
    float intensity2 = 0.0f;

    DirectX::XMFLOAT4 effectColor1 = {};
    DirectX::XMFLOAT4 effectColor2 = {};

    float contourIntensity = 0;
    float contourSensitive = 0;
    DirectX::XMFLOAT2 dummy;
  };

public:
  DirectX::XMFLOAT2 uvScrollDir1 = {};
  DirectX::XMFLOAT2 uvScrollDir2 = {};

  float simulateSpeed1 = 1.0f;
  float simulateSpeed2 = 1.0f;

  DirectX::XMFLOAT2 uvScale1 = { 1.0f,1.0f };
  DirectX::XMFLOAT2 uvScale2 = { 1.0f,1.0f };

  DirectX::XMFLOAT4 effectColor1 = { 1,1,1,1 };
  DirectX::XMFLOAT4 effectColor2 = { 1,1,1,1 };

  float intensity1 = 1.0f;
  float intensity2 = 1.0f;

  float contourIntensity = 1.0;
  float contourSensitive = 500.0;

private:
  inline static std::unique_ptr<ConstantBuffer<Buffer>> m_constants;

};

class FakeInteriorConstants :public BaseConstants
{
public:
  FakeInteriorConstants();

  void DrawGui()override;

  void UpdateConstantBuffer(ID3D11DeviceContext* dc)override;

public:
  struct Buffer {
    DirectX::XMFLOAT2 tiling;
    float offset = 0.0f;
    float reflectionAmount;
  };

public:
  DirectX::XMFLOAT2 tiling = { 1.0f,1.0f };
  float offset = 0.0f;
  float reflectionAmount = 0.5f;

private:
  inline static std::unique_ptr<ConstantBuffer<Buffer>> m_constants;

};
