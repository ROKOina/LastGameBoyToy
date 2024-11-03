#pragma once

#include "Graphics/Shader.h"
#include "Graphics/ConstantBuffer.h"
#include "Graphics/Graphics.h"
#include "Graphics/Model/Model.h"

#define MAX_BONES 256

class ModelShader
{
public:
    ModelShader(SHADER_ID_MODEL shader);
    ~ModelShader() {};

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

public:
    //�I�u�W�F�N�g�̃R���X�^���g�o�b�t�@
    struct objectconstants
    {
        DirectX::XMFLOAT4X4 BoneTransforms[MAX_BONES] = {};
    };

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
    std::unique_ptr<ConstantBuffer<objectconstants>> m_objectconstants;
    std::unique_ptr<ConstantBuffer<subsetconstants>> m_subsetconstants;
    std::unique_ptr<ConstantBuffer<m_general>> m_generalconstants;

    Microsoft::WRL::ComPtr<ID3D11VertexShader>       m_vertexshader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>        m_pixelshader;
    Microsoft::WRL::ComPtr<ID3D11GeometryShader>     m_geometryshader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>        m_inputlayout;
};
