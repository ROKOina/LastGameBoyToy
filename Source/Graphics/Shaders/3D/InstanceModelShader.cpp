#include "InstanceModelShader.h"

//�R���X�g���N�^
InstanceModelShader::InstanceModelShader(SHADER_ID_MODEL shader, int count)
{
    Graphics& Graphics = Graphics::Instance();

    //�t�@�C����
    const char* VSPath = nullptr;
    const char* PSPath = nullptr;
    const char* GSPath = nullptr;

    //�I�u�W�F�N�g���ǂꂾ���o���������߂�
    m_instancecount = count;

    //�I�����ꂽ�̂��w�肳���
    switch (shader)
    {
    case SHADER_ID_MODEL::DEFAULT:
        VSPath = { "Shader\\InstancingVS.cso" };
        PSPath = { "Shader\\DefaltPS.cso" };
        break;
    case SHADER_ID_MODEL::DEFERRED:
        VSPath = { "Shader\\InstancingVS.cso" };
        PSPath = { "Shader\\DeferredSetupPS.cso" };
        break;
    case SHADER_ID_MODEL::BLACK:
        VSPath = { "Shader\\InstancingVS.cso" };
        PSPath = { "Shader\\BlackPS.cso" };
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
    //���̓��C�A�E�g
    D3D11_INPUT_ELEMENT_DESC IED[] =
    {
        // ���͗v�f�̐ݒ�
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "I_ROTATION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "I_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "I_SCALE", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    };
    CreateVsFromCso(Graphics.GetDevice(), VSPath, m_vertexshader.GetAddressOf(), m_inputlayout.ReleaseAndGetAddressOf(), IED, ARRAYSIZE(IED));
    VSPath = { "Shader\\InstanceShadowVS.cso" };
    CreateVsFromCso(Graphics.GetDevice(), VSPath, m_vertexshaderShadow.GetAddressOf(), m_inputlayout.ReleaseAndGetAddressOf(), IED, ARRAYSIZE(IED));

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
        m_objectconstants = std::make_unique<ConstantBuffer<objectconstants>>(Graphics.GetDevice());
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
    ReplaceBufferContents(m_instancedata.Get(), sizeof(Instance) * m_instancecount, m_cpuinstancedata.get());
}
void InstanceModelShader::ShadowBegin(ID3D11DeviceContext* dc, BLENDSTATE blendmode, DEPTHSTATE depthmode, RASTERIZERSTATE rasterizermode)
{
    //�V�F�[�_�[�̃Z�b�g
    dc->VSSetShader(m_vertexshaderShadow.Get(), nullptr, 0);
    dc->GSSetShader(m_geometryshaderShadow.Get(), nullptr, 0);
    dc->PSSetShader(NULL, NULL, 0);
    dc->IASetInputLayout(m_inputlayout.Get());

    Graphics& Graphics = Graphics::Instance();
    const float blendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

    //�����_�[�X�e�[�g�̐ݒ�i���[�v�̊O��1�񂾂��ݒ�j
    dc->OMSetBlendState(Graphics.GetBlendState(blendmode), blendFactor, 0xFFFFFFFF);
    dc->OMSetDepthStencilState(Graphics.GetDepthStencilState(depthmode), 1);
    dc->RSSetState(Graphics.GetRasterizerState(rasterizermode));

    // �C���X�^���V���O�̃f�[�^���i�[
    ReplaceBufferContents(m_instancedata.Get(), sizeof(Instance) * m_instancecount, m_cpuinstancedata.get());
}

void InstanceModelShader::SetBuffer(ID3D11DeviceContext* dc, const std::vector<Model::Node>& nodes, const ModelResource::Mesh& mesh)
{
    //�萔�o�b�t�@�Ƀ��b�V����transform�𑗂�
    DirectX::XMStoreFloat4x4(&m_objectconstants->data.transform, DirectX::XMLoadFloat4x4(&nodes.at(mesh.nodeIndex).worldTransform));

    //�R���X�^���g�o�b�t�@�̍X�V
    m_objectconstants->Activate(dc, (int)CB_INDEX::OBJECT, true, true, false, false, false, false);

    UINT strides[] = { sizeof(ModelResource::Vertex), sizeof(Instance) };
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
    m_subsetconstants->Activate(dc, (int)CB_INDEX::SUBSET, true, true, false, false, false, false);

    //�I�u�W�F�N�g���Ɏg�������萔�o�b�t�@
    m_generalconstants->data.outlineColor = subset.material->outlineColor;
    m_generalconstants->data.outlineintensity = subset.material->outlineintensity;
    m_generalconstants->data.statictype = subset.material->statictype;
    m_generalconstants->Activate(dc, (int)CB_INDEX::GENERAL, false, true, false, false, false, false);

    //�V�F�[�_�[���\�[�X�r���[�ݒ�
    dc->PSSetShaderResources(0, std::size(subset.material->shaderResourceView), subset.material->shaderResourceView[0].GetAddressOf());

    //�C���X�^���X���g������p��Draw
    dc->DrawIndexedInstanced(subset.indexCount, m_instancecount, subset.startIndex, 0, 0);
}
void InstanceModelShader::ShadowSetSubset(ID3D11DeviceContext* dc, const ModelResource::Subset& subset)
{
    dc->DrawIndexedInstanced(subset.indexCount, m_instancecount, subset.startIndex, 0, 0);
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
    // �C���X�^���X���Ƃ̃f�[�^��ҏW�ł���悤�ɂ���
    for (int i = 0; i < m_instancecount; ++i)
    {
        Instance& instance = m_cpuinstancedata[i]; // �e�C���X�^���X�ւ̎Q��

        // �C���X�^���X�̕ҏW�pUI���쐬
        ImGui::PushID(i); // �C���X�^���X���ƂɃ��j�[�NID��t����
        if (ImGui::CollapsingHeader(("Instance " + std::to_string(i)).c_str()))
        {
            // �ʒu (Position) �̕ҏW
            ImGui::DragFloat3("Position", &instance.position.x, 0.1f);

            // ��] (Rotation) �̕ҏW�i�N�H�[�^�j�I���j
            ImGui::DragFloat4("Rotation", &instance.quaternion.x, 0.1f);

            // �X�P�[�� (Scale) �̕ҏW
            ImGui::DragFloat3("Scale", &instance.scale.x, 0.1f);
        }
        ImGui::PopID();
    }

    // �C���X�^���X�o�b�t�@���X�V���邽�߂̃f�[�^���ēx�o�b�t�@�ɑ��M
    ReplaceBufferContents(m_instancedata.Get(), sizeof(Instance) * m_instancecount, m_cpuinstancedata.get());
}

// �C���X�^���V���O�̃f�[�^���o�b�t�@�Ɋi�[
void InstanceModelShader::CreateBuffer()
{
    Graphics& Graphics = Graphics::Instance();

    // �C���X�^���V���O�̃f�[�^���o�b�t�@�Ɋi�[
    CD3D11_BUFFER_DESC bufferDesc(sizeof(Instance) * m_instancecount, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
    bufferDesc.StructureByteStride = sizeof(Instance);
    HRESULT hr = Graphics.GetDevice()->CreateBuffer(&bufferDesc, nullptr, m_instancedata.ReleaseAndGetAddressOf());
    if (FAILED(hr))
    {
        // �G���[����
        throw std::runtime_error("Failed to create instance buffer");
    }
    m_cpuinstancedata.reset(new Instance[m_instancecount]);
}

//�ʒu�A�p���A�傫�����o�b�t�@�Ɋi�[
void InstanceModelShader::ReplaceBufferContents(ID3D11Buffer* buffer, size_t bufferSize, const void* data)
{
    D3D11_MAPPED_SUBRESOURCE mapped;
    Graphics& graphics = Graphics::Instance();
    graphics.GetDeviceContext()->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    memcpy(mapped.pData, data, bufferSize);
    graphics.GetDeviceContext()->Unmap(buffer, 0);
}