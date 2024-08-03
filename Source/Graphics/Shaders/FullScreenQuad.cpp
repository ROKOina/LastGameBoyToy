#include "FullScreenQuad.h"
#include "Graphics/Graphics.h"
#include "Shader.h"
#include "Misc.h"
#include "Texture.h"

//�R���X�g���N�^
FullScreenQuad::FullScreenQuad()
{
    ID3D11Device* device = Graphics::Instance().GetDevice();

    //���_�V�F�[�_�[
    CreateVsFromCso(device, "Shader\\FullScreenQuadVS.cso", m_embeddedvertexshader.ReleaseAndGetAddressOf(), nullptr, nullptr, 0);

    // �s�N�Z���V�F�[�_�[
    CreatePsFromCso(device, "Shader\\FullScreenQuadPS.cso", m_embeddedpixelshader.ReleaseAndGetAddressOf());
}

//���X�̃s�N�Z���V�F�[�_���Q�Ƃ��ăL���b�V������֐�(�t���X�N���[���̎l�p�`��`�悵�āA�w�肳�ꂽ�s�N�Z���V�F�[�_�⃊�\�[�X�r���[��ݒ肵�`�����������Ƃɖ߂��Ă���)
void FullScreenQuad::Blit(ID3D11DeviceContext* immediate_context, ID3D11ShaderResourceView* const* shader_resource_views, uint32_t start_slot, uint32_t num_views, ID3D11PixelShader* replaced_pixel_shader)
{
    // ���݂̃s�N�Z���V�F�[�_�[�̃��\�[�X�r���[���L���b�V��
    immediate_context->PSGetShaderResources(start_slot, num_views, m_cachedShaderResourceViews);

    // �V�����s�N�Z���V�F�[�_�[�̃��\�[�X�r���[���Z�b�g
    immediate_context->PSSetShaderResources(start_slot, num_views, shader_resource_views);

    // ���_�o�b�t�@, �v���~�e�B�u�̃g�|���W�[, ���̓��C�A�E�g��ݒ�
    immediate_context->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
    immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    immediate_context->IASetInputLayout(NULL);

    // ���ߍ��܂ꂽ���_�V�F�[�_�[��ݒ�
    immediate_context->VSSetShader(m_embeddedvertexshader.Get(), 0, 0);
    replaced_pixel_shader ? immediate_context->PSSetShader(replaced_pixel_shader, 0, 0) : immediate_context->PSSetShader(m_embeddedpixelshader.Get(), 0, 0);

    // �s�N�Z���V�F�[�_�[�̃��\�[�X�r���[���ēx�ݒ�
    immediate_context->PSSetShaderResources(start_slot, num_views, shader_resource_views);

    immediate_context->RSSetState(Graphics::Instance().GetRasterizerState(RASTERIZERSTATE::SOLID_CULL_NONE));

    // 4�̒��_����Ȃ�O�p�`�X�g���b�v
    immediate_context->Draw(4, 0);

    // ���̃s�N�Z���V�F�[�_�[�̃��\�[�X�r���[�𕜌�
    immediate_context->PSSetShaderResources(start_slot, num_views, m_cachedShaderResourceViews);
}