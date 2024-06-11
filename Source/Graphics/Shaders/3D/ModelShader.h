#pragma once

#include "Graphics/Shaders/Shader.h"
#include "Graphics/Shaders/ConstantBuffer.h"
#include "Graphics/Graphics.h"
#include "Graphics/Model/Model.h"

#define MAX_BONES 256

class ModelShader
{
public:
    ModelShader(int shader);
    ~ModelShader() {};

    //�`�揉���ݒ�
    void Begin(ID3D11DeviceContext* dc, int blendmode);

    //�`�揈��
    void SetBuffer(ID3D11DeviceContext* dc, const std::vector<Model::Node>& nodes, const ModelResource::Mesh& mesh);

    //�T�u�Z�b�g���̕`��
    void SetSubset(ID3D11DeviceContext* dc, const ModelResource::Subset& subset);

    //�`��I������
    void End(ID3D11DeviceContext* dc);

public:
    //�I�u�W�F�N�g�̃R���X�^���g�o�b�t�@
    struct objectconstants
    {
        DirectX::XMFLOAT4X4 BoneTransforms[MAX_BONES] = {};
    };
    std::unique_ptr<ConstantBuffer<objectconstants>> m_objectconstants;

    //�T�u�Z�b�g�̃R���X�^���g�o�b�t�@
    struct subsetconstants
    {
        DirectX::XMFLOAT4	color = { 1.0f, 1.0f, 1.0f, 1.0f };
        DirectX::XMFLOAT3   emissivecolor = { 1.0f,1.0f,1.0f };
        float               emissiveintensity = 0;
        float               Metalness = 0;
        float               Roughness = 0;
        DirectX::XMFLOAT2 dummy = {};
    };
    std::unique_ptr<ConstantBuffer<subsetconstants>> m_subsetconstants;

    //�G�t�F�N�g�̃R���X�^���g�o�b�t�@
    struct effectconstants
    {
      float simulateTime1 = 0.0f;
      float simulateTime2 = 0.0f;
      float simulateTime3 = 0.0f;

      float waveEffectRange = 3.0f;
    };
    std::unique_ptr<ConstantBuffer<effectconstants>> m_effectconstants;

public:
    enum MODELSHADER
    {
        DEFALT,
        DEFERRED,
        BLACK,
        AREA_EFFECT_CIRCLE,
        MAX
    };

private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader>       m_vertexshader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>        m_pixelshader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>        m_inputlayout;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_skybox;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_diffuseiem;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_specularpmrem;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_lutggx;
};
