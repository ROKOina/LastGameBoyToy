#include "VariousShaderConstants.h"
#include "imgui.h"
#include "Graphics/Graphics.h"

EffectConstants::EffectConstants()
{
  Graphics& graphics = Graphics::Instance();

  if (m_effectconstants == nullptr) {
    m_effectconstants = std::make_unique<ConstantBuffer<Buffer>>(graphics.GetDevice());
  }
}

void EffectConstants::DrawGui()
{
  ImGui::DragFloat("SimulateSpeed1", &simulateSpeed1, 0.1f, 0.0f);
  ImGui::DragFloat("SimulateSpeed2", &simulateSpeed2, 0.1f, 0.0f);
  ImGui::DragFloat("SimulateSpeed3", &simulateSpeed3, 0.1f, 0.0f);

  ImGui::DragFloat("WaveEffectRange", &waveEffectRange, 0.01f, 0.0f, 1.0f);
  ImGui::ColorEdit3("WaveColor", &waveEffectColor.x, ImGuiColorEditFlags_None);
  ImGui::DragFloat("ColorAlpha", &waveEffectColor.w, 0.01f, 0.0f);
  ImGui::DragFloat("WaveIntensity", &waveEffectIntensity, 0.1f, 0.0f);
}

void EffectConstants::Update(const float& elapsedTime)
{
  auto& data = m_effectconstants->data;

  data.simulateTime1 += elapsedTime * simulateSpeed1;
  data.simulateTime2 += elapsedTime * simulateSpeed2;
  data.simulateTime3 += elapsedTime * simulateSpeed3;
  data.waveEffectRange = waveEffectRange;

  data.waveEffectColor = waveEffectColor;
  data.waveEffectIntensity = waveEffectIntensity;
}

void EffectConstants::UpdateConstantBuffer(ID3D11DeviceContext* dc)
{
  m_effectconstants->Activate(dc, (int)CB_INDEX::VARIOUS, false, true, false, false, false, false);
}

SciFiGateConstants::SciFiGateConstants()
{
  Graphics& graphics = Graphics::Instance();

  if (m_constants == nullptr) {
    m_constants = std::make_unique<ConstantBuffer<Buffer>>(graphics.GetDevice());
  }
}

void SciFiGateConstants::DrawGui()
{
  ImGui::DragFloat("SimulateSpeed1", &simulateSpeed1, 0.01f, 0.0f);
  ImGui::DragFloat("SimulateSpeed2", &simulateSpeed2, 0.01f, 0.0f);

  ImGui::DragFloat2("uvScrollDir1", &uvScrollDir1.x, 0.01f, 0.0f, 1.0f);
  ImGui::DragFloat2("uvScrollDir2", &uvScrollDir2.x, 0.01f, 0.0f, 1.0f);

  ImGui::DragFloat2("uvScale1", &uvScale1.x, 0.001f, 0.0f);
  ImGui::DragFloat2("uvScale2", &uvScale2.x, 0.001f, 0.0f);

  ImGui::ColorEdit4("effectColor1", &effectColor1.x);
  ImGui::ColorEdit4("effectColor2", &effectColor2.x);

  ImGui::DragFloat("Intensity1", &intensity1, 0.01f, 0.0f, 80.0f);
  ImGui::DragFloat("Intensity2", &intensity2, 0.01f, 0.0f, 80.0f);

  ImGui::DragFloat("contourIntensity", &contourIntensity, 0.01f, 0.0f, 80.0f);
  ImGui::DragFloat("contourSensitive", &contourSensitive, 0.1f, 0.0f, 1000.0f);
}

void SciFiGateConstants::Update(const float& elapsedTime)
{
  auto& data = m_constants->data;

  data.simulateTime1 += elapsedTime * simulateSpeed1;
  data.simulateTime2 += elapsedTime * simulateSpeed2;

  data.uvScale1 = uvScale1;
  data.uvScale2 = uvScale2;

  data.uvScrollDir1 = uvScrollDir1;
  data.uvScrollDir2 = uvScrollDir2;

  data.effectColor1 = effectColor1;
  data.effectColor2 = effectColor2;

  data.intensity1 = intensity1;
  data.intensity2 = intensity2;

  data.contourIntensity = contourIntensity;
  data.contourSensitive = contourSensitive;
}

void SciFiGateConstants::UpdateConstantBuffer(ID3D11DeviceContext* dc)
{
  m_constants->Activate(dc, (int)CB_INDEX::VARIOUS, false, true, false, false, false, false);
}

FakeInteriorConstants::FakeInteriorConstants()
{
  Graphics& graphics = Graphics::Instance();

  if (m_constants == nullptr) {
    m_constants = std::make_unique<ConstantBuffer<Buffer>>(graphics.GetDevice());
  }
}

void FakeInteriorConstants::DrawGui()
{
  ImGui::DragFloat2("Tiling", &tiling.x, 0.1f, 0.0f);
  ImGui::DragFloat("Offset", &offset, 0.01f, 0.0f);
  ImGui::DragFloat("Reflection", &reflectionAmount, 0.01f, 0.0f);
}

void FakeInteriorConstants::UpdateConstantBuffer(ID3D11DeviceContext* dc)
{
  auto& data = m_constants->data;
  data.tiling = tiling;
  data.offset = offset;
  data.reflectionAmount = reflectionAmount;

  m_constants->Activate(dc, (int)CB_INDEX::VARIOUS, false, true, false, false, false, false);
}
