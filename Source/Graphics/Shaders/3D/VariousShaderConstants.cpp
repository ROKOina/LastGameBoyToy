#include "VariousShaderConstants.h"
#include "imgui.h"
#include "Graphics/Graphics.h"

EffectConstants::EffectConstants()
{
  Graphics& graphics = Graphics::Instance();

  m_effectconstants = std::make_unique<ConstantBuffer<Buffer>>(graphics.GetDevice());
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
  m_effectconstants->Activate(dc, 3, false, true, false, false, false, false);
}
