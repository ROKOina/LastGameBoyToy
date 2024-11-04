#pragma once

#include "Component/System\Component.h"
#include "Graphics\Model\Model.h"
#include "Graphics\Graphics.h"
#include "Graphics/3DShader/InstanceModelShader.h"
#include "Graphics/3DShader/VariousShaderConstants.h"

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

    // �}�e���A���̂ݓǂݍ���
    void LoadMaterial(const char* filename);

    /// <summary>
    /// �o�b�`�`��I�u�W�F�N�g�̐��� by��
    /// </summary>
    /// <param name="isChildObject"> �q�I�u�W�F�N�g�Ƃ��Đ������邩</param>
    /// <param name=""> TRUE = �q�I�u�W�F�N�g</param>
    /// <param name=""> FALSE = �e�q�֌W�Ȃ��̃I�u�W�F�N�g</param>
    GameObj CreateInstance(bool isChildObject);

    template<class T>
    std::shared_ptr<T> SetVariousConstant() {
      std::shared_ptr<T> p = std::make_shared<T>();
      variousConstant = p;
      return p;
    }

    template<class T>
    std::shared_ptr<T> GetVariousConstant() {
      std::shared_ptr<T> p = std::dynamic_pointer_cast<T>(variousConstant);
      if (p == nullptr) return nullptr;
      return p;
    }

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
    SHADER_ID_MODEL     shaderID = SHADER_ID_MODEL::DEFERRED;
    bool m_shadowrender = true;

    std::shared_ptr<BaseConstants> variousConstant = nullptr;
};