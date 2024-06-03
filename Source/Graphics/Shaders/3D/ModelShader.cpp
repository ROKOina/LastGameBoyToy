#include "ModelShader.h"
#include "Graphics/Shaders/Texture.h"

//������
ModelShader::ModelShader(int shader)
{
    Graphics& Graphics = Graphics::Instance();

    //�t�@�C����
    const char* VSPath = nullptr;
    const char* PSPath = nullptr;

    //�I�����ꂽ�̂��w�肳���
    switch (shader)
    {
    case MODELSHADER::Defalt:
        VSPath = { "Shader\\DefaltVS.cso" };
        PSPath = { "Shader\\DefaltPS.cso" };
        break;
    case MODELSHADER::Deferred:
        VSPath = { "Shader\\DefaltVS.cso" };
        PSPath = { "Shader\\PBR+IBL_Unity.cso" };
        break;
    }

    // ���_�V�F�[�_�[
    //���̓��C�A�E�g
    D3D11_INPUT_ELEMENT_DESC IED[] =
    {
        // ���͗v�f�̐ݒ�
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "BONES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    CreateVsFromCso(Graphics.GetDevice(), VSPath, VertexShader.GetAddressOf(), InputLayout.ReleaseAndGetAddressOf(), IED, ARRAYSIZE(IED));

    // �s�N�Z���V�F�[�_�[
    CreatePsFromCso(Graphics.GetDevice(), PSPath, PixelShader.GetAddressOf());

    // �萔�o�b�t�@
    {
        // �I�u�W�F�N�g�p�̃R���X�^���g�o�b�t�@,�T�u�Z�b�g�p
        ObjectConstants = std::make_unique<ConstantBuffer<object_constants>>(Graphics.GetDevice());
        SubsetConstants = std::make_unique<ConstantBuffer<subset_constants>>(Graphics.GetDevice());
    }

    //IBL��p�̃e�N�X�`���ǂݍ���
    {
        D3D11_TEXTURE2D_DESC texture2d_desc{};
        LoadTextureFromFile(Graphics.GetDevice(), L".\\Data\\Texture\\snowy_hillside_4k.DDS", skybox.GetAddressOf(), &texture2d_desc);
        LoadTextureFromFile(Graphics.GetDevice(), L".\\Data\\Texture\\diffuse_iem.dds", diffuseiem.GetAddressOf(), &texture2d_desc);
        LoadTextureFromFile(Graphics.GetDevice(), L".\\Data\\Texture\\specular_pmrem.dds", specularpmrem.GetAddressOf(), &texture2d_desc);
        LoadTextureFromFile(Graphics.GetDevice(), L".\\Data\\Texture\\lut_ggx.DDS", lutggx.GetAddressOf(), &texture2d_desc);
    }
}

//�`�揈��
void ModelShader::Render(ID3D11DeviceContext* dc, Model* model)
{
    Graphics& Graphics = Graphics::Instance();
    const float blendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

    //�����_�[�X�e�[�g�̐ݒ�i���[�v�̊O��1�񂾂��ݒ�j
    dc->OMSetBlendState(Graphics.GetBlendState(BLENDSTATE::MULTIPLERENDERTARGETS), blendFactor, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(Graphics.GetDepthStencilState(DEPTHSTATE::ZT_ON_ZW_ON), 1);
    dc->RSSetState(Graphics.GetRasterizerState(RASTERIZERSTATE::SOLID_CULL_BACK));

    //�V�F�[�_�[�̃Z�b�g
    dc->VSSetShader(VertexShader.Get(), nullptr, 0);
    dc->PSSetShader(PixelShader.Get(), nullptr, 0);
    dc->IASetInputLayout(InputLayout.Get());

    //IBL��p�̃e�N�X�`���Z�b�g
    dc->PSSetShaderResources(6, 1, skybox.GetAddressOf());
    dc->PSSetShaderResources(7, 1, diffuseiem.GetAddressOf());
    dc->PSSetShaderResources(8, 1, specularpmrem.GetAddressOf());
    dc->PSSetShaderResources(9, 1, lutggx.GetAddressOf());

    //���f�����
    const ModelResource* resource = model->GetResource();
    const std::vector<Model::Node>& nodes = model->GetNodes();

    for (const ModelResource::Mesh& mesh : resource->GetMeshes())
    {
        // ���b�V���p�萔�o�b�t�@�X�V
        if (mesh.nodeIndices.size() > 0)
        {
            for (size_t i = 0; i < mesh.nodeIndices.size(); ++i)
            {
                DirectX::XMMATRIX worldTransform = DirectX::XMLoadFloat4x4(&nodes.at(mesh.nodeIndices.at(i)).worldTransform);
                DirectX::XMMATRIX offsetTransform = DirectX::XMLoadFloat4x4(&mesh.offsetTransforms.at(i));
                DirectX::XMMATRIX boneTransform = offsetTransform * worldTransform;
                DirectX::XMStoreFloat4x4(&ObjectConstants->data.BoneTransforms[i], boneTransform);
            }
        }
        else
        {
            ObjectConstants->data.BoneTransforms[0] = nodes.at(mesh.nodeIndex).worldTransform;
        }

        ObjectConstants->Activate(dc, 1, true, true, false, false, false, false);

        UINT stride = sizeof(ModelResource::Vertex);
        UINT offset = 0;
        dc->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
        dc->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
        dc->IASetInputLayout(InputLayout.Get());
        dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        //�T�u�Z�b�g���̍X�V
        for (const ModelResource::Subset& subset : mesh.subsets)
        {
            //�R���X�^���g�o�b�t�@�̏��
            SubsetConstants->data.color = subset.material->color;
            SubsetConstants->data.emissivecolor = subset.material->emissivecolor;
            SubsetConstants->data.emissiveintensity = subset.material->emissiveintensity;
            SubsetConstants->data.Metalness = subset.material->Metalness;
            SubsetConstants->data.Roughness = subset.material->Roughness;
            SubsetConstants->Activate(dc, 2, true, true, false, false, false, false);

            //�V�F�[�_�[���\�[�X�r���[�ݒ�
            for (int i = 0; i < 6; ++i)
            {
                dc->PSSetShaderResources(i, 1, subset.material->shaderResourceView[i].GetAddressOf());
            }

            dc->DrawIndexed(subset.indexCount, subset.startIndex, 0);
        }
    }

    //������Ă�����
    dc->VSSetShader(NULL, NULL, 0);
    dc->PSSetShader(NULL, NULL, 0);
}