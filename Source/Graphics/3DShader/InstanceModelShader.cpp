#include "InstanceModelShader.h"
#include "Component/System/TransformCom.h"

#define MAX_STATIC_BATCH (300) // �o�b�`�`�悷��ő吔 by��

//�R���X�g���N�^
InstanceModelShader::InstanceModelShader(SHADER_ID_MODEL shader, int count)
{
    Graphics& Graphics = Graphics::Instance();

    //�t�@�C����
    const char* VSPath = nullptr;
    const char* PSPath = nullptr;
    const char* GSPath = nullptr;

    //�I�����ꂽ�̂��w�肳���
    switch (shader)
    {
    case SHADER_ID_MODEL::DEFERRED:
        VSPath = { "Shader\\InstancingVS.cso" };
        PSPath = { "Shader\\DeferredSetupPS.cso" };
        break;
    case SHADER_ID_MODEL::AREA_EFFECT_CIRCLE:
        VSPath = { "Shader\\InstancingVS.cso" };
        PSPath = { "Shader\\AreaEffectCirclePS.cso" };
        break;
    case SHADER_ID_MODEL::FAKE_DEPTH:
        VSPath = { "Shader\\FakeDepthVS.cso" };
        PSPath = { "Shader\\FakeDepthPS.cso" };
        break;
    case SHADER_ID_MODEL::SCI_FI_GATE:
        VSPath = { "Shader\\InstancingVS.cso" };
        PSPath = { "Shader\\SciFiGatePS.cso" };
        break;
    case SHADER_ID_MODEL::SILHOUETTE:
        VSPath = { "Shader\\InstancingVS.cso" };
        PSPath = { "Shader\\SilhouettePS.cso" };
        break;

    default:
        assert(!"������Shader�ɑz�肳��Ă��Ȃ��l��������Ă���");
    }

    // ���_�V�F�[�_�[
    {
        //���̓��C�A�E�g
        D3D11_INPUT_ELEMENT_DESC IED[] =
        {
            // ���͗v�f�̐ݒ�
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

            // �e�C���X�^���X�̎p���s��
            // 4x4�̃��[���h�s��
            { "TRANSFORM",0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
            { "TRANSFORM",1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
            { "TRANSFORM",2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
            { "TRANSFORM",3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        };
        CreateVsFromCso(Graphics.GetDevice(), VSPath, m_vertexshader.GetAddressOf(), m_inputlayout.ReleaseAndGetAddressOf(), IED, ARRAYSIZE(IED));
    }
    // ���_�V�F�[�_�[ ( �e�p )
    {
        //���̓��C�A�E�g
        D3D11_INPUT_ELEMENT_DESC IED[] =
        {
            // ���͗v�f�̐ݒ�
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

            // �e�C���X�^���X�̎p���s��
            // 4x4�̃��[���h�s��
            { "TRANSFORM",0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 4 },
            { "TRANSFORM",1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 4 },
            { "TRANSFORM",2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 4 },
            { "TRANSFORM",3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 4 },
        };
        VSPath = { "Shader\\InstanceShadowVS.cso" };
        CreateVsFromCso(Graphics.GetDevice(), VSPath, m_vertexshaderShadow.GetAddressOf(), m_inputlayoutShadow.ReleaseAndGetAddressOf(), IED, ARRAYSIZE(IED));
    }

    // �s�N�Z���V�F�[�_�[�ƃW�I���g���V�F�[�_�[
    if (shader != SHADER_ID_MODEL::SHADOW)
    {
        CreatePsFromCso(Graphics.GetDevice(), PSPath, m_pixelshader.GetAddressOf());
    }
    {
        GSPath = { "Shader\\ShadowGS.cso" };
        CreateGsFromCso(Graphics.GetDevice(), GSPath, m_geometryshaderShadow.GetAddressOf());
    }

    // �萔�o�b�t�@
    {
        // �I�u�W�F�N�g�p�̃R���X�^���g�o�b�t�@,�T�u�Z�b�g�p
        m_subsetconstants = std::make_unique<ConstantBuffer<subsetconstants>>(Graphics.GetDevice());
        m_generalconstants = std::make_unique<ConstantBuffer<m_general>>(Graphics.GetDevice());
    }

    // �C���X�^���V���O�̃f�[�^���o�b�t�@�Ɋi�[
    CreateBuffer();
}

//�`��ݒ�
void InstanceModelShader::Begin(ID3D11DeviceContext* dc, BLENDSTATE blendmode, DEPTHSTATE depthmode, RASTERIZERSTATE rasterizermode)
{
    //�V�F�[�_�[�̃Z�b�g
    dc->VSSetShader(m_vertexshader.Get(), nullptr, 0);
    dc->PSSetShader(m_pixelshader.Get(), nullptr, 0);
    dc->IASetInputLayout(m_inputlayout.Get());

    Graphics& Graphics = Graphics::Instance();
    const float blendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

    //�����_�[�X�e�[�g�̐ݒ�i���[�v�̊O��1�񂾂��ݒ�j
    dc->OMSetBlendState(Graphics.GetBlendState(blendmode), blendFactor, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(Graphics.GetDepthStencilState(depthmode), 1);
    dc->RSSetState(Graphics.GetRasterizerState(rasterizermode));

    // �C���X�^���V���O�̃f�[�^���i�[
    ReplaceBufferContents();
}
void InstanceModelShader::ShadowBegin(ID3D11DeviceContext* dc, BLENDSTATE blendmode, DEPTHSTATE depthmode, RASTERIZERSTATE rasterizermode)
{
    //�V�F�[�_�[�̃Z�b�g
    dc->VSSetShader(m_vertexshaderShadow.Get(), nullptr, 0);
    dc->GSSetShader(m_geometryshaderShadow.Get(), nullptr, 0);
    dc->PSSetShader(NULL, NULL, 0);
    dc->IASetInputLayout(m_inputlayoutShadow.Get());

    Graphics& Graphics = Graphics::Instance();
    const float blendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

    //�����_�[�X�e�[�g�̐ݒ�i���[�v�̊O��1�񂾂��ݒ�j
    dc->OMSetBlendState(Graphics.GetBlendState(blendmode), blendFactor, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(Graphics.GetDepthStencilState(depthmode), 1);
    dc->RSSetState(Graphics.GetRasterizerState(rasterizermode));

    // �C���X�^���V���O�̃f�[�^���i�[
    ReplaceBufferContents();
}

void InstanceModelShader::SetBuffer(ID3D11DeviceContext* dc, const std::vector<Model::Node>& nodes, const ModelResource::Mesh& mesh)
{
    UINT strides[] = { sizeof(ModelResource::Vertex), sizeof(DirectX::XMFLOAT4X4) };
    UINT offsets[] = { 0, 0 };
    ID3D11Buffer* Buffers[] = { mesh.vertexBuffer.Get(), m_instancedata.Get() };
    dc->IASetVertexBuffers(0, _countof(strides), Buffers, strides, offsets);
    dc->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

//�T�u�Z�b�g���̕`��
void InstanceModelShader::SetSubset(ID3D11DeviceContext* dc, const ModelResource::Subset& subset)
{
    //�R���X�^���g�o�b�t�@�̏��
    m_subsetconstants->data.color = subset.material->color;
    m_subsetconstants->data.emissivecolor = subset.material->emissivecolor;
    m_subsetconstants->data.emissiveintensity = subset.material->emissiveintensity;
    m_subsetconstants->data.Metalness = subset.material->Metalness;
    m_subsetconstants->data.Roughness = subset.material->Roughness;
    m_subsetconstants->data.alpha = subset.material->alpha;
    m_subsetconstants->Activate(dc, (int)CB_INDEX::SUBSET, true, true, false, false, false, false);

    //�I�u�W�F�N�g���Ɏg�������萔�o�b�t�@
    m_generalconstants->data.outlineColor = subset.material->outlineColor;
    m_generalconstants->data.outlineintensity = subset.material->outlineintensity;
    m_generalconstants->Activate(dc, (int)CB_INDEX::GENERAL, false, true, false, false, false, false);

    //�V�F�[�_�[���\�[�X�r���[�ݒ�
    dc->PSSetShaderResources(0, std::size(subset.material->shaderResourceView), subset.material->shaderResourceView[0].GetAddressOf());

    //�C���X�^���X���g������p��Draw
    dc->DrawIndexedInstanced(subset.indexCount, iModelTransforms.size(), subset.startIndex, 0, 0);
}
void InstanceModelShader::ShadowSetSubset(ID3D11DeviceContext* dc, const ModelResource::Subset& subset)
{
    dc->DrawIndexedInstanced(subset.indexCount, iModelTransforms.size() * 4, subset.startIndex, 0, 0);
}

//�`��I������
void InstanceModelShader::End(ID3D11DeviceContext* dc)
{
    dc->IASetInputLayout(nullptr);

    //������Ă�����
    dc->VSSetShader(NULL, NULL, 0);
    dc->PSSetShader(NULL, NULL, 0);
    dc->GSSetShader(NULL, NULL, 0);
}

//imgui
void InstanceModelShader::ImGui()
{
}

// �C���X�^���V���O�̃f�[�^���o�b�t�@�Ɋi�[
void InstanceModelShader::CreateBuffer()
{
    Graphics& Graphics = Graphics::Instance();

    float instanceCount = MAX_STATIC_BATCH;
    // �C���X�^���V���O�̃f�[�^���o�b�t�@�Ɋi�[
    CD3D11_BUFFER_DESC bufferDesc(sizeof(DirectX::XMFLOAT4X4) * instanceCount, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
    bufferDesc.StructureByteStride = sizeof(DirectX::XMFLOAT4X4);
    HRESULT hr = Graphics.GetDevice()->CreateBuffer(&bufferDesc, nullptr, m_instancedata.ReleaseAndGetAddressOf());
    if (FAILED(hr))
    {
        // �G���[����
        throw std::runtime_error("Failed to create instance buffer");
    }
}

void InstanceModelShader::RemoveInstance(std::weak_ptr<TransformCom> transform)
{
    std::vector<std::weak_ptr<TransformCom>>::iterator it = std::find_if(iModelTransforms.begin(), iModelTransforms.end(),
        [&](const std::weak_ptr<TransformCom>& ptr1) {
            return ptr1.lock() == transform.lock();
        });

    assert(it != iModelTransforms.end());

    iModelTransforms.erase(it);
}

//�ʒu�A�p���A�傫�����o�b�t�@�Ɋi�[
void InstanceModelShader::ReplaceBufferContents()
{
    Graphics& graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = graphics.GetDeviceContext();

    D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
    HRESULT hr = dc->Map(m_instancedata.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
    _ASSERT_EXPR(SUCCEEDED(hr), HRTrace(hr));

    DirectX::XMFLOAT4X4* data = reinterpret_cast<DirectX::XMFLOAT4X4*>(mapped_subresource.pData);

    assert(iModelTransforms.size() <= MAX_STATIC_BATCH);
    for (int i = 0; i < iModelTransforms.size(); ++i)
    {
        data[i] = iModelTransforms[i].lock()->GetWorldTransform();
    }
    dc->Unmap(m_instancedata.Get(), 0);
}