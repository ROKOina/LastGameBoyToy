#include "FullScreenQuad.h"
#include "Graphics/Graphics.h"
#include "Shader.h"
#include "Misc.h"
#include "Texture.h"

//�R���X�g���N�^
FullScreenQuad::FullScreenQuad(ID3D11Device* device)
{
    //���_�V�F�[�_�[
    CreateVsFromCso(device, "Shader\\FullScreenQuadVS.cso", m_embeddedvertexshader.ReleaseAndGetAddressOf(), nullptr, nullptr, 0);

    // �s�N�Z���V�F�[�_�[
    CreatePsFromCso(device, "Shader\\FullScreenQuadPS.cso", m_embeddedpixelshader.ReleaseAndGetAddressOf());

    //�X�J�C�}�b�v�p�s�N�Z���V�F�[�_�[
    CreatePsFromCso(device, "Shader\\SkyBoxPS.cso", m_skymappixelshader.ReleaseAndGetAddressOf());

    //SKYMAP�p�e�N�X�`����ǂݍ���
    D3D11_TEXTURE2D_DESC texture2d_desc{};
    LoadTextureFromFile(device, "Data\\Texture\\snowy_hillside_4k.DDS", m_skymap.GetAddressOf(), &texture2d_desc);
}

//���X�̃s�N�Z���V�F�[�_���Q�Ƃ��ăL���b�V������֐�(�t���X�N���[���̎l�p�`��`�悵�āA�w�肳�ꂽ�s�N�Z���V�F�[�_�⃊�\�[�X�r���[��ݒ肵�`�����������Ƃɖ߂��Ă���)
void FullScreenQuad::Blit(ID3D11DeviceContext* immediate_context, ID3D11ShaderResourceView* const* shader_resource_views, uint32_t start_slot, uint32_t num_views, ID3D11PixelShader* replaced_pixel_shader)
{
    //���݂̃s�N�Z���V�F�[�_�[�̃��\�[�X�r���[���L���b�V��
    ID3D11ShaderResourceView* cached_shader_resource_views[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT]{};
    immediate_context->PSGetShaderResources(start_slot, num_views, cached_shader_resource_views);

    //�V�����s�N�Z���V�F�[�_�[�̃��\�[�X�r���[���Z�b�g
    immediate_context->PSSetShaderResources(start_slot, num_views, shader_resource_views);

    //���_�o�b�t�@,�v���~�e�B�u�̃g�|���W�[,���̓��C�A�E�g��ݒ�
    immediate_context->IASetVertexBuffers(0, 0, NULL, NULL, NULL);
    immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    immediate_context->IASetInputLayout(NULL);

    //���ߍ��܂ꂽ���_�V�F�[�_�[��ݒ�
    immediate_context->VSSetShader(m_embeddedvertexshader.Get(), 0, 0);
    replaced_pixel_shader ? immediate_context->PSSetShader(replaced_pixel_shader, 0, 0) : immediate_context->PSSetShader(m_embeddedpixelshader.Get(), 0, 0);

    //�s�N�Z���V�F�[�_�[�̃��\�[�X�r���[���ēx�ݒ�
    immediate_context->PSSetShaderResources(start_slot, num_views, shader_resource_views);

    //4�̒��_����Ȃ�O�p�`�X�g���b�v
    immediate_context->Draw(4, 0);

    //���̃s�N�Z���V�F�[�_�[�̃��\�[�X�r���[�𕜌�
    immediate_context->PSSetShaderResources(start_slot, num_views, cached_shader_resource_views);
    for (ID3D11ShaderResourceView* cached_shader_resource_view : cached_shader_resource_views)
    {
        if (cached_shader_resource_view) cached_shader_resource_view->Release();
    }
}

//skymap
void FullScreenQuad::SkyMap()
{
    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();
    dc->OMSetDepthStencilState(Graphics::Instance().GetDepthStencilState(DEPTHSTATE::ZT_OFF_ZW_OFF), 1);
    dc->RSSetState(Graphics::Instance().GetRasterizerState(RASTERIZERSTATE::SOLID_CULL_NONE));
    Blit(graphics.GetDeviceContext(), m_skymap.GetAddressOf(), 10, 1, m_skymappixelshader.Get());
}