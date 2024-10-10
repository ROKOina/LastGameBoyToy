#pragma once

#include "System\Component.h"
#include "Graphics\Model\Model.h"
#include "Graphics\Graphics.h"
#include "Graphics/Shaders/3D/InstanceModelShader.h"

class InstanceRenderer :public Component
{
    // �R���|�[�l���g�I�[�o�[���C�h
public:
    InstanceRenderer(SHADER_ID_MODEL id, int maxinstance, BLENDSTATE blendmode, DEPTHSTATE depthmode = DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE rasterizermode = RASTERIZERSTATE::SOLID_CULL_BACK, bool shadowrender = true);
    ~InstanceRenderer() {}

    // ���O�擾
    const char* GetName() const override { return "InstanceRenderer"; }

    // �J�n����
    void Start() override {};

    // �`��
    void Render();

    // �X�V����
    void Update(float elapsedTime) override;

    //�e�`��
    void ShadowRender();

    // GUI�`��
    void OnGUI() override;

    // ���f���̓ǂݍ���
    void LoadModel(const char* filename);

public:

    //id�擾
    SHADER_ID_MODEL GetShaderMode() { return shaderID; }

    // ���f���̎擾
    Model* GetModel() const { return model_.get(); }

private:

    std::unique_ptr<Model>	model_;
    std::unique_ptr<InstanceModelShader>m_instancemodelshader;
    RASTERIZERSTATE m_rasterizerState = RASTERIZERSTATE::SOLID_CULL_BACK;
    BLENDSTATE m_blend = BLENDSTATE::MULTIPLERENDERTARGETS;
    DEPTHSTATE m_depth = DEPTHSTATE::ZT_ON_ZW_ON;
    SHADER_ID_MODEL     shaderID = SHADER_ID_MODEL::DEFAULT;
    bool m_shadowrender = true;
    int count = 0;
};