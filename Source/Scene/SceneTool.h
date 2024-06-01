#pragma once

#include "Scene.h"
#include "Graphics/PostEffect.h"
#include "Camera/CameraController.h"
#include "Model/ModelResource.h"
#include "Model/FbxModelResource.h"
#include "Model/Model.h"
#include <imgui.h>
#include <ImGuizmo.h>

//�c�[���̃V�[��
class SceneTool :public Scene
{
public:
    SceneTool() {};
    ~SceneTool()override {};

    //������
    void Initialize()override;

    //�I����
    void Finalize()override {};

    //�X�V����
    void Update(float elapsedTime)override;

    //�`�揈��
    void Render()override;

private:

    //Debug�`��
    void DebugRender();

    // �O���b�h�`��
    void DrawGrid(ID3D11DeviceContext* context, int subdivisions, float scale);

    // �M�Y���`��
    void DrawGuizmo(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection, Model::Node* node);
    void DrawRootGuizmo(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection, DirectX::XMFLOAT3& rootPos);
    //
        // GUI�`��
    void DrawGUI();

    // ���j���[GUI�`��
    void DrawMenuGUI();

    // �m�[�hGUI�`��
    void DrawNodeGUI(Model::Node* node);

    // �q�G�����L�[GUI�`��
    void DrawHierarchyGUI();

    // �v���p�e�BGUI�`��
    void DrawPropertyGUI();

    // �A�j���[�V����GUI�`��
    void DrawAnimationGUI();

    // �^�C�����C��GUI�`��
    void DrawTimelineGUI();

    // �}�e���A��GUI�`��
    void DrawMaterialGUI();

    // ���f���t�@�C�����J��
    void OpenModelFile();

    //�}�e���A���t�@�C�����J��
    void OpenMaterial();

    // ���f���t�@�C����ۑ�
    void SaveModelFile();

    //�}�e���A���t�@�C����ۑ�
    void SaveMaterialFile();

    //�A�j���[�V�����t�@�C����ۑ�
    void SaveAnimationFile();

    //�m�[�h��ۑ�
    void SaveNodeFile();

    //�A�j���[�V�����t�@�C�����J��
    void OpenAnimationFile();

    //�����̃A�j���[�V�����t�@�C�����J��
    void ExisitingOpenAnimationFile();

    //�m�[�h�t�@�C�����J��
    void OpenNodeFile();

    // ���f������
    void ImportModel(const char* filename);

    //�}�e���A������
    void ImportMaterial(const char* filename);

    //�����̃A�j���[�V��������
    void ImportExisitingAnimation(const char* filename);

    //�m�[�h����
    void ImportNode(const char* filename);

    // �A�j���[�V��������
    void ImportAnimation(const char* filename);

    // ���f���o��
    void ExportModel(const char* filename);

    //�}�e���A���o��
    void ExportMaterial(const char* filename);

    //�A�j���[�V�����o��
    void ExportAnimation(const char* filename);

    //�m�[�h�o��
    void ExportNode(const char* filename);

    //���[�g���[�V�����X�V����
    void UpdateRootMotion();

    // �I���A�j���[�V�������擾
    ModelResource::Animation* GetSelectionAnimation();

    // �I���}�e���A�����擾
    ModelResource::Material* GetSelectionMaterial();

private:
    CameraController					camera;
    std::shared_ptr<FbxModelResource>	modelResource;
    std::unique_ptr<Model>				model;
    std::unique_ptr<PostEffect>         posteffect;
    std::string							modelFilePath;
    Model::Node* selectionNode = nullptr;
    DirectX::XMFLOAT3                   rootposition = { 0,0,0 };
    int									selectionAnimationIndex = -1;
    int									selectionMaterialIndex = -1;
    int                                 selectionAnimationevent = -1;
    int                                 shaderid = 1;
    bool								hiddenHierarchy = false;
    bool								hiddenProperty = false;
    bool								hiddenAnimation = false;
    bool                                hiddenData = false;
    bool								hiddenTimeline = false;
    bool                                rootGizmo = false;
    bool                                rootEditing = false;
    bool								animationLoop = false;
    bool                                nodeselect = false;
    ImGuizmo::OPERATION					guizmoOperation = ImGuizmo::TRANSLATE;
    ImGuizmo::MODE						guizmoMode = ImGuizmo::LOCAL;
};