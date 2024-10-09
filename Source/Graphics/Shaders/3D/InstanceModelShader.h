#pragma once

#include "Graphics/Shaders/Shader.h"
#include "Graphics/Shaders/ConstantBuffer.h"
#include "Graphics/Graphics.h"
#include "Graphics/Model/Model.h"

class InstanceModelShader
{
public:
    InstanceModelShader(SHADER_ID_MODEL shader);
    ~InstanceModelShader() {};

    //�`�揉���ݒ�
    void Begin(ID3D11DeviceContext* dc, BLENDSTATE blendmode, DEPTHSTATE depthmode, RASTERIZERSTATE rasterizermode);
    void ShadowBegin(ID3D11DeviceContext* dc, BLENDSTATE blendmode, DEPTHSTATE depthmode, RASTERIZERSTATE rasterizermode);

    //�`�揈��
    void SetBuffer(ID3D11DeviceContext* dc, const std::vector<Model::Node>& nodes, const ModelResource::Mesh& mesh);

    //�T�u�Z�b�g���̕`��
    void SetSubset(ID3D11DeviceContext* dc, const ModelResource::Subset& subset);
    void ShadowSetSubset(ID3D11DeviceContext* dc, const ModelResource::Subset& subset);

    //�`��I������
    void End(ID3D11DeviceContext* dc);

private:

    //�ʒu�A�p���A�傫�����o�b�t�@�Ɋi�[
    void ReplaceBufferContents(ID3D11Buffer* buffer, size_t bufferSize, const void* data);

public:

    //�ʒu
    void SetPosition(const DirectX::XMFLOAT3& position, const int& index) { m_cpuinstancedata[index].position = position; }
    const DirectX::XMFLOAT3& GetPosition(int index) const { return m_cpuinstancedata[index].position; }

    //�傫��
    void SetScale(const DirectX::XMFLOAT3& scale, const int& index) { m_cpuinstancedata[index].scale = scale; }
    const DirectX::XMFLOAT3& GetScale(int index) const { return m_cpuinstancedata[index].scale; }

    //��]
    void SetQuaternion(const DirectX::XMFLOAT4& quaternion, const int& index) { m_cpuinstancedata[index].quaternion = quaternion; }
    const DirectX::XMFLOAT4& GetQuaternion(int index) const { return m_cpuinstancedata[index].quaternion; }

    //������
    void SetCount(const int& index) { m_instancecount = index; }
    const int& GetCount() const { return m_instancecount; }

private:

    //�C���X�^���V���O�̏��
    struct Instance
    {
        DirectX::XMFLOAT4 quaternion = { 0,0,0,1 };
        DirectX::XMFLOAT3 position{ 0,0,0 };
        DirectX::XMFLOAT3 scale{ 1.0f,1.0f,1.0f };
    };
    std::unique_ptr<Instance[]> m_cpuinstancedata;

    //�I�u�W�F�N�g�̃R���X�^���g�o�b�t�@
    struct objectconstants
    {
        DirectX::XMFLOAT4X4 transform = {};
    };

    //�T�u�Z�b�g�̃R���X�^���g�o�b�t�@
    struct subsetconstants
    {
        DirectX::XMFLOAT4	color = { 1.0f, 1.0f, 1.0f, 1.0f };
        DirectX::XMFLOAT3 emissivecolor = { 1.0f,1.0f,1.0f };
        float             emissiveintensity = 0;
        float             Metalness = 0;
        float             Roughness = 0;
        DirectX::XMFLOAT2 dummy = {};
    };

    //�ėp�̃R���X�^���g�o�b�t�@
    struct m_general
    {
        DirectX::XMFLOAT3 outlineColor = { 0,0,0 };
        float outlineintensity = 1.0f;
        int statictype = 0;
        DirectX::XMFLOAT3 generaldummy = {};
    };

private:
    std::unique_ptr<ConstantBuffer<objectconstants>> m_objectconstants;
    std::unique_ptr<ConstantBuffer<subsetconstants>> m_subsetconstants;
    std::unique_ptr<ConstantBuffer<m_general>> m_generalconstants;

    Microsoft::WRL::ComPtr<ID3D11VertexShader>       m_vertexshader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>        m_pixelshader;
    Microsoft::WRL::ComPtr<ID3D11GeometryShader>     m_geometryshader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>        m_inputlayout;
    Microsoft::WRL::ComPtr<ID3D11Buffer>m_instancedata;
    int m_instancecount = 1;
};