#pragma once

#include "Scene.h"
#include "Graphics/PostEffect.h"
#include "Camera/CameraController.h"
#include "Model/ModelResource.h"
#include "Model/FbxModelResource.h"
#include "Model/Model.h"
#include <imgui.h>
#include <ImGuizmo.h>

//ツールのシーン
class SceneTool :public Scene
{
public:
    SceneTool() {};
    ~SceneTool()override {};

    //初期化
    void Initialize()override;

    //終了化
    void Finalize()override {};

    //更新処理
    void Update(float elapsedTime)override;

    //描画処理
    void Render()override;

private:

    //Debug描画
    void DebugRender();

    // グリッド描画
    void DrawGrid(ID3D11DeviceContext* context, int subdivisions, float scale);

    // ギズモ描画
    void DrawGuizmo(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection, Model::Node* node);
    void DrawRootGuizmo(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection, DirectX::XMFLOAT3& rootPos);
    //
        // GUI描画
    void DrawGUI();

    // メニューGUI描画
    void DrawMenuGUI();

    // ノードGUI描画
    void DrawNodeGUI(Model::Node* node);

    // ヒエラルキーGUI描画
    void DrawHierarchyGUI();

    // プロパティGUI描画
    void DrawPropertyGUI();

    // アニメーションGUI描画
    void DrawAnimationGUI();

    // タイムラインGUI描画
    void DrawTimelineGUI();

    // マテリアルGUI描画
    void DrawMaterialGUI();

    // モデルファイルを開く
    void OpenModelFile();

    //マテリアルファイルを開く
    void OpenMaterial();

    // モデルファイルを保存
    void SaveModelFile();

    //マテリアルファイルを保存
    void SaveMaterialFile();

    //アニメーションファイルを保存
    void SaveAnimationFile();

    //ノードを保存
    void SaveNodeFile();

    //アニメーションファイルを開く
    void OpenAnimationFile();

    //既存のアニメーションファイルを開く
    void ExisitingOpenAnimationFile();

    //ノードファイルを開く
    void OpenNodeFile();

    // モデル入力
    void ImportModel(const char* filename);

    //マテリアル入力
    void ImportMaterial(const char* filename);

    //既存のアニメーション入力
    void ImportExisitingAnimation(const char* filename);

    //ノード入力
    void ImportNode(const char* filename);

    // アニメーション入力
    void ImportAnimation(const char* filename);

    // モデル出力
    void ExportModel(const char* filename);

    //マテリアル出力
    void ExportMaterial(const char* filename);

    //アニメーション出力
    void ExportAnimation(const char* filename);

    //ノード出力
    void ExportNode(const char* filename);

    //ルートモーション更新処理
    void UpdateRootMotion();

    // 選択アニメーションを取得
    ModelResource::Animation* GetSelectionAnimation();

    // 選択マテリアルを取得
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