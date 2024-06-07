#include "FullScreenQuad.h"
#include "Graphics.h"
#include "Shader.h"
#include "System/Misc.h"
#include "Texture.h"

static FullScreenQuad* instance = nullptr;

FullScreenQuad::FullScreenQuad(ID3D11Device* device)
{
    //�C���X�^���X�|�C���^�ݒ�
    instance = this;

    //���_�V�F�[�_�[
    create_vs_from_cso(device, "Shader\\FullScreenQuadVS.cso", embedded_vertex_shader.ReleaseAndGetAddressOf(), nullptr, nullptr, 0);

    // �s�N�Z���V�F�[�_�[
    create_ps_from_cso(device, "Shader\\FullScreenQuadPS.cso", embedded_pixel_shader.ReleaseAndGetAddressOf());

    //�X�J�C�}�b�v�p�s�N�Z���V�F�[�_�[
    create_ps_from_cso(device, "Shader\\SkyBoxPS.cso", skymappixelshader.ReleaseAndGetAddressOf());

    //SKYMAP�p�e�N�X�`����ǂݍ���
    D3D11_TEXTURE2D_DESC texture2d_desc{};
   // load_texture_from_file(device, L".\\Data\\Texture\\snowy_hillside_4k.DDS", skymap.GetAddressOf(), &texture2d_desc);
}

//�C���X�^���X�擾
FullScreenQuad& FullScreenQuad::Instance()
{
    return *instance;
}

//���X�̃s�N�Z���V�F�[�_���Q�Ƃ��ăL���b�V������֐�(�t���X�N���[���̎l�p�`��`�悵�āA�w�肳�ꂽ�s�N�Z���V�F�[�_�⃊�\�[�X�r���[��ݒ肵�`�����������Ƃɖ߂��Ă���)
void FullScreenQuad::blit(ID3D11DeviceContext* immediate_context, ID3D11ShaderResourceView* const* shader_resource_views, uint32_t start_slot, uint32_t num_views, ID3D11PixelShader* replaced_pixel_shader)
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
    immediate_context->VSSetShader(embedded_vertex_shader.Get(), 0, 0);
    replaced_pixel_shader ? immediate_context->PSSetShader(replaced_pixel_shader, 0, 0) : immediate_context->PSSetShader(embedded_pixel_shader.Get(), 0, 0);

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
    dc->OMSetDepthStencilState(Graphics::Instance().GetDepthStencilState(DEPTHSTATE::ZT_OFF_ZW_OFF), 0);
    dc->RSSetState(Graphics::Instance().GetRasterizerState(RASTERIZERSTATE::SOLID_CULL_NONE));
    blit(graphics.GetDeviceContext(), skymap.GetAddressOf(), 10, 1, skymappixelshader.Get());
}