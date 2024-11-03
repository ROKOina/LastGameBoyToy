#pragma once

#include "Graphics/Shaders/Shader.h"
#include "Graphics/Shaders/ConstantBuffer.h"
#include "Graphics/Graphics.h"
#include "Graphics/Model/Model.h"

class TransformCom;

class InstanceModelShader
{
public:
    InstanceModelShader(SHADER_ID_MODEL shader, int count);
    ~InstanceModelShader() {};

    //�`�揉���ݒ�
    void Begin(ID3D11DeviceContext* dc, BLENDSTATE blendmode, DEPTHSTATE depthmode, RASTERIZERSTATE rasterizermode);
    void ShadowBegin(ID3D11DeviceContext* dc, BLENDSTATE blendmode, DEPTHSTATE depthmode, RASTERIZERSTATE rasterizermode);

    //�`�揈��
    void SetBuffer(ID3D11DeviceContext* dc, const std::vector<Model::Node>& nodes, const ModelResource::Mesh& mesh);

    //�T�u�Z�b�g���̕`��
    void SetSubset(ID3D11DeviceContext* dc, const ModelResource::Subset& subset, const std::vector<ModelResource::Material>& materials);
    void ShadowSetSubset(ID3D11DeviceContext* dc, const ModelResource::Subset& subset);

    //�`��I������
    void End(ID3D11DeviceContext* dc);

    // �p�����o�b�t�@�Ɋi�[
    void ReplaceBufferContents();

    //imgui
    void ImGui();

    //�o�b�t�@�[�쐬
    void CreateBuffer();

public: // by ��
    // �o�b�`�`��Ɏg�p����p���̒ǉ�
    void AddInstance(std::weak_ptr<TransformCom> transform) { iModelTransforms.push_back(transform); }
    // �o�b�`�`��Ɏg�p����p���̍폜
    void RemoveInstance(std::weak_ptr<TransformCom> transform);

    const int& GetInstanceCount() const { return iModelTransforms.size(); }

private:
    //�T�u�Z�b�g�̃R���X�^���g�o�b�t�@
    struct subsetconstants
    {
        DirectX::XMFLOAT4	color = { 1.0f, 1.0f, 1.0f, 1.0f };
        DirectX::XMFLOAT3 emissivecolor = { 1.0f,1.0f,1.0f };
        float             emissiveintensity = 0;
        float             Metalness = 0;
        float             Roughness = 0;
        float             alpha = 0.0f;
        float             dummy = {};
    };

    //�ėp�̃R���X�^���g�o�b�t�@
    struct m_general
    {
        DirectX::XMFLOAT3 outlineColor = { 0,0,0 };
        float outlineintensity = 1.0f;
    };

private:
    std::unique_ptr<ConstantBuffer<subsetconstants>> m_subsetconstants;
    std::unique_ptr<ConstantBuffer<m_general>>       m_generalconstants;

    Microsoft::WRL::ComPtr<ID3D11VertexShader>       m_vertexshader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>        m_pixelshader;
    Microsoft::WRL::ComPtr<ID3D11GeometryShader>     m_geometryshader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>        m_inputlayout;
    Microsoft::WRL::ComPtr<ID3D11Buffer>             m_instancedata;
    Microsoft::WRL::ComPtr<ID3D11VertexShader>       m_vertexshaderShadow;
    Microsoft::WRL::ComPtr<ID3D11GeometryShader>     m_geometryshaderShadow;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>        m_inputlayoutShadow;

    // �o�b�`�`�悷��I�u�W�F�N�g�̎p�� by��
    std::vector<std::weak_ptr<TransformCom>> iModelTransforms;
};