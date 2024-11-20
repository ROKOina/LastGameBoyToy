#pragma once

#include "Component\System\Component.h"
#include "Graphics\Model\Model.h"
#include "Graphics\Graphics.h"
#include "Graphics/3DShader/ModelShader.h"
#include "Graphics/3DShader/VariousShaderConstants.h"

class RendererCom : public Component
{
    // �R���|�[�l���g�I�[�o�[���C�h
public:
    RendererCom(SHADER_ID_MODEL id, BLENDSTATE blendmode, DEPTHSTATE depthmode = DEPTHSTATE::ZT_ON_ZW_ON, RASTERIZERSTATE rasterizermode = RASTERIZERSTATE::SOLID_CULL_BACK, bool shadowrender = true, bool silhoutterender = false);
    ~RendererCom() {}

    // ���O�擾
    const char* GetName() const override { return "Renderer"; }

    // �J�n����
    void Start() override;

    // �`��
    void Render();

    // �X�V����
    void Update(float elapsedTime) override;

    //�e�`��
    void ShadowRender();

    //�V���G�b�g�`��
    void SilhoutteRender();

    //guizmo
    void BoneGuizmo(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection);

    // GUI�`��
    void OnGUI() override;

    // ���f���̓ǂݍ���
    void LoadModel(const char* filename);

    // �}�e���A���̂ݓǂݍ���
    void LoadMaterial(const char* filename);

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

private:

    //ModelResource�Ŏg���Ă���}�e���A�����̒l��ۑ�
    struct MaterialParameter
    {
        DirectX::XMFLOAT3 outlineColor = { 0,0,0 };
        float outlineintensity = 1.0f;
        float dissolveThreshold = 0.0f; // �f�B�]���u臒l
        DirectX::XMFLOAT3 dissolveEdgeColor = { 1,1,1 }; // �G�b�W�̐F
        float alpha = 1.0f;
    };
    MaterialParameter p;

public:

    // ���f���̎擾
    Model* GetModel() const { return model_.get(); }
    std::string GetModelPath() { return modelFilePath; }

    //�o�E���f�B���O�{�b�N�X
    DirectX::XMFLOAT3 GetBounds() { return bounds; }
    DirectX::XMFLOAT3 GetBoundsMin() { return BoundsMin; }

    //�V�F�[�_�[�擾
    SHADER_ID_MODEL GetShaderMode() { return shaderID; }

    //�X���b�h�擾
    void JoinThred() { future.get(); }

    //�}�e���A���p�����[�^�̐ݒ�֐�
    void SetOutlineColor(const DirectX::XMFLOAT3& color) { p.outlineColor = color; }
    void SetOutlineIntensity(float intensity) { p.outlineintensity = intensity; }
    void SetDissolveThreshold(float threshold) { p.dissolveThreshold = threshold; }
    void SetDissolveEdgeColor(const DirectX::XMFLOAT3& color) { p.dissolveEdgeColor = color; }
    void SetAlpha(float a) { p.alpha = a; }

private:
    void ModelInitialize(const char* filename);

    // �}�e���A����I������ImGui
    void MaterialSelector();

    //�}�e���A���擾�֐�
    ModelResource::Material* GetSelectionMaterial();
    std::vector<ModelResource::Material*> RendererCom::GetAllMaterials();

    void TextureGui(ModelResource::Material*);

    // �ҏW�����}�e���A���t�@�C���������o��
    void ExportMaterialFile();

    //�}�e���A���̕ύX
    void ChangeMaterialParameter();

private:
    std::unique_ptr<Model>	model_;
    std::unique_ptr<ModelShader> m_modelshader;
    std::unique_ptr<ModelShader>m_shadow;
    std::unique_ptr<ModelShader>m_silhoutte;
    bool m_shadowrender = true;
    bool m_silhoutterender = true;
    RASTERIZERSTATE m_rasterizerState = RASTERIZERSTATE::SOLID_CULL_BACK;
    BLENDSTATE m_blend = BLENDSTATE::MULTIPLERENDERTARGETS;
    DEPTHSTATE m_depth = DEPTHSTATE::ZT_ON_ZW_ON;
    std::shared_ptr<BaseConstants> variousConstant = nullptr;
    DirectX::XMFLOAT3       BoundsMin;
    DirectX::XMFLOAT3       BoundsMax;
    DirectX::XMFLOAT3 bounds;
    SHADER_ID_MODEL     shaderID = SHADER_ID_MODEL::DEFERRED;
    int							selectionMaterialIndex = -1;
    bool						hiddenProperty = false;
    std::string					modelFilePath;
    std::string					filePathDriveToModel = "";

    //���f���ǂݍ��݂��X���b�h��
    std::future<void> future;
};