#pragma once

#include <wrl.h>
#include <d3d11.h>
#include <stdint.h>

class FullScreenQuad
{
private:
  FullScreenQuad();
  virtual ~FullScreenQuad() = default;

public:
  // �V���O���g��
  static FullScreenQuad& Instance() {
    static FullScreenQuad instance;
    return instance;
  }

  //���X�̃s�N�Z���V�F�[�_���Q�Ƃ��ăL���b�V������֐�
  void Blit(ID3D11DeviceContext* immediate_contextbool, ID3D11ShaderResourceView* const* shader_resource_views, uint32_t start_slot, uint32_t num_views, ID3D11PixelShader* replaced_pixel_shader = nullptr);

private:
  Microsoft::WRL::ComPtr<ID3D11VertexShader>       m_embeddedvertexshader;
  Microsoft::WRL::ComPtr<ID3D11PixelShader>        m_embeddedpixelshader;
  Microsoft::WRL::ComPtr<ID3D11DepthStencilView>   m_depthstencilview;
  Microsoft::WRL::ComPtr<ID3D11PixelShader>        m_skymappixelshader;
};