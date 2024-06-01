//#include <sstream>
#include <shlwapi.h>
#include <algorithm>
#include "SceneTool.h"
#include "Graphics/Graphics.h"
#include "Graphics/ModelShader.h"
#include "Mathf/TransformUtils.h"
#include "System/Framework.h"
#include "System/Dialog.h"
#include "Light/Light.h"
#include "Light/LightManager.h"
#include "Camera/Camera.h"

//初期化
void SceneTool::Initialize()
{
    //ポストエフェクト生成
    posteffect = std::make_unique<PostEffect>();

    //コンスタントバッファの初期化
    ConstantBufferInitialize();

    //平行光源を追加
    Light* mainDirectionalLight = new Light(LightType::Directional);
    mainDirectionalLight->SetDirection({ -0.5f, -0.5f, 0 });
    mainDirectionalLight->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
    LightManager::Instance().Register(mainDirectionalLight);
}

//更新処理
void SceneTool::Update(float elapsedTime)
{
    //触る時に動いてしまうのを対策する為
    rootEditing = false;

    // モデル更新
    if (model != nullptr)
    {
        model->UpdateAnimation(elapsedTime);
        model->UpdateTransform(DirectX::XMMatrixIdentity());
        model->UpdateTransform(DirectX::XMMatrixTranslation(rootposition.x, rootposition.y, rootposition.z));
        UpdateRootMotion();
        model->CopyModel();
    }

    //カメラ更新
    camera.Update();
}

//描画処理
void SceneTool::Render()
{
    //viewportの設定
    ViewPortInitialize();

    //コンスタントバッファの更新
    ConstantBufferUpdate();

    //ポストエフェクトの前処理
    posteffect->BeforeSet();

    // ライトの定数バッファを更新
    LightManager::Instance().UpdateConstatBuffer();

    //モデルの描画
    if (model != nullptr)
    {
        Graphics::Instance().GetModelShader(shaderid)->Render(Graphics::Instance().GetDeviceContext(), model.get());
    }

    //Debug描画
    DebugRender();

    //ポストエフェクトの後処理
    posteffect->AfterSet();

    // GUI描画
    DrawGUI();
}

// ギズモ描画
void SceneTool::DrawGuizmo(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection, Model::Node* node)
{
    // 選択ノードの行列を操作する
    ImGuizmo::Manipulate(
        &view._11, &projection._11,	// ビュー＆プロジェクション行列
        guizmoOperation,			// 操作
        guizmoMode,					// 空間
        &node->worldTransform._11,	// 操作するワールド行列
        nullptr);

    // 操作したワールド行列をローカル行列に反映
    if (node->parent != nullptr)
    {
        // 親のワールド逆行列と自身のワールド行列を乗算することで自身のローカル行列を算出できる
        DirectX::XMMATRIX ParentWorldTransform = DirectX::XMLoadFloat4x4(&node->parent->worldTransform);
        DirectX::XMMATRIX ParentInverseTransform = DirectX::XMMatrixInverse(nullptr, ParentWorldTransform);
        DirectX::XMMATRIX WorldTransform = DirectX::XMLoadFloat4x4(&node->worldTransform);
        DirectX::XMMATRIX LocalTransform = DirectX::XMMatrixMultiply(WorldTransform, ParentInverseTransform);
        DirectX::XMStoreFloat4x4(&node->localTransform, LocalTransform);
    }
    else
    {
        node->localTransform = node->worldTransform;
    }

    // ローカル行列からSRT値に反映する
    TransformUtils::MatrixToTransformation(node->localTransform, &node->scale, &node->rotate, &node->translate);
}

// ルートモーション用ギズモ描画
void SceneTool::DrawRootGuizmo(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection, DirectX::XMFLOAT3& rootPos)
{
    DirectX::XMFLOAT4X4 rootTransform = {
   1,0,0,0,
   0,1,0,0,
   0,0,1,0,
   rootPos.x,rootPos.y,rootPos.z,1,
    };

    // 選択ノードの行列を操作する
    rootEditing |= ImGuizmo::Manipulate(
        &view._11, &projection._11,    // ビュー＆プロジェクション行列
        guizmoOperation,            // 操作
        guizmoMode,                    // 空間
        &rootTransform._11,    // 操作するワールド行列
        nullptr);

    // ローカル行列からSRT値に反映する
    TransformUtils::MatrixToTransformation(rootTransform, nullptr, nullptr, &rootPos);
}

//グリッド描画
void SceneTool::DrawGrid(ID3D11DeviceContext* context, int subdivisions, float scale)
{
    int numLines = (subdivisions + 1) * 2;
    int vertexCount = numLines * 2;

    float corner = 0.5f;
    float step = 1.0f / static_cast<float>(subdivisions);

    int index = 0;
    float s = -corner;

    const DirectX::XMFLOAT4 white = DirectX::XMFLOAT4(1, 1, 1, 1);

    LineRenderer* lineRenderer = Graphics::Instance().GetLineRenderer();
    // Create vertical lines
    float scaling = static_cast<float>(subdivisions) * scale;
    DirectX::XMMATRIX M = DirectX::XMMatrixScaling(scaling, scaling, scaling);
    DirectX::XMVECTOR V, P;
    DirectX::XMFLOAT3 position;
    for (int i = 0; i <= subdivisions; i++)
    {
        V = DirectX::XMVectorSet(s, 0, corner, 0);
        P = DirectX::XMVector3TransformCoord(V, M);
        DirectX::XMStoreFloat3(&position, P);
        lineRenderer->AddVertex(position, white);

        V = DirectX::XMVectorSet(s, 0, -corner, 0);
        P = DirectX::XMVector3TransformCoord(V, M);
        DirectX::XMStoreFloat3(&position, P);
        lineRenderer->AddVertex(position, white);

        s += step;
    }

    // Create horizontal lines
    s = -corner;
    for (int i = 0; i <= subdivisions; i++)
    {
        V = DirectX::XMVectorSet(corner, 0, s, 0);
        P = DirectX::XMVector3TransformCoord(V, M);
        DirectX::XMStoreFloat3(&position, P);
        lineRenderer->AddVertex(position, white);

        V = DirectX::XMVectorSet(-corner, 0, s, 0);
        P = DirectX::XMVector3TransformCoord(V, M);
        DirectX::XMStoreFloat3(&position, P);
        lineRenderer->AddVertex(position, white);

        s += step;
    }

    // X軸
    {
        const DirectX::XMFLOAT4 red = DirectX::XMFLOAT4(1, 0, 0, 1);
        V = DirectX::XMVectorSet(0, 0, 0, 0);
        P = DirectX::XMVector3TransformCoord(V, M);
        DirectX::XMStoreFloat3(&position, P);
        lineRenderer->AddVertex(position, red);

        V = DirectX::XMVectorSet(corner, 0, 0, 0);
        P = DirectX::XMVector3TransformCoord(V, M);
        DirectX::XMStoreFloat3(&position, P);
        lineRenderer->AddVertex(position, red);
    }

    // Y軸
    {
        const DirectX::XMFLOAT4 green = DirectX::XMFLOAT4(0, 1, 0, 1);
        V = DirectX::XMVectorSet(0, 0, 0, 0);
        P = DirectX::XMVector3TransformCoord(V, M);
        DirectX::XMStoreFloat3(&position, P);
        lineRenderer->AddVertex(position, green);

        V = DirectX::XMVectorSet(0, corner, 0, 0);
        P = DirectX::XMVector3TransformCoord(V, M);
        DirectX::XMStoreFloat3(&position, P);
        lineRenderer->AddVertex(position, green);
    }

    // Z軸
    {
        const DirectX::XMFLOAT4 blue = DirectX::XMFLOAT4(0, 0, 1, 1);
        V = DirectX::XMVectorSet(0, 0, 0, 0);
        P = DirectX::XMVector3TransformCoord(V, M);
        DirectX::XMStoreFloat3(&position, P);
        lineRenderer->AddVertex(position, blue);

        V = DirectX::XMVectorSet(0, 0, corner, 0);
        P = DirectX::XMVector3TransformCoord(V, M);
        DirectX::XMStoreFloat3(&position, P);
        lineRenderer->AddVertex(position, blue);
    }
}

//Debug描画
void SceneTool::DebugRender()
{
    Graphics& Graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = Graphics.GetDeviceContext();

    //ギズモの位置調整
    ImGuizmo::BeginFrame();
    ImGuizmo::SetOrthographic(false);
    RECT r;
    ::GetWindowRect(Framework::Instance().GetHwnd(), &r);
    ImGuizmo::SetRect(r.left, r.top, r.right - r.left, r.bottom - r.top);

    // ギズモ描画
    if (!rootGizmo)
    {
        if (selectionNode != nullptr)
        {
            DrawGuizmo(sc->data.view, sc->data.projection, selectionNode);
        }
    }
    else
    {
        DrawRootGuizmo(sc->data.view, sc->data.projection, rootposition);
    }

    // グリッド描画
    DrawGrid(dc, 20, 10.0f);

    //ライン描画
    Graphics.GetLineRenderer()->Render(dc, sc->data.view, sc->data.projection);
}

// GUI描画
void SceneTool::DrawGUI()
{
    DrawMenuGUI();
    DrawHierarchyGUI();
    DrawPropertyGUI();
    DrawAnimationGUI();
    DrawTimelineGUI();
    DrawMaterialGUI();
    posteffect->PostEffectImGui();
}

// メニューGUI描画
void SceneTool::DrawMenuGUI()
{
    if (ImGui::BeginMainMenuBar())
    {
        // ファイルメニュー
        if (ImGui::BeginMenu("OpenFile"))
        {
            bool check = false;
            if (ImGui::MenuItem("Open Model", "", &check))
            {
                OpenModelFile();
            }

            if (ImGui::MenuItem("Open Material", "", &check))
            {
                OpenMaterial();
            }

            if (ImGui::MenuItem("Open Node", "", &check))
            {
                OpenNodeFile();
            }

            if (ImGui::MenuItem("Open Animation", "", &check))
            {
                OpenAnimationFile();
            }

            if (ImGui::MenuItem("Open ExitingAnimation", "", &check))
            {
                ExisitingOpenAnimationFile();
            }
            ImGui::EndMenu();
        }

        //セーブ(個別)
        if (ImGui::BeginMenu("SaveFile"))
        {
            bool check = false;

            if (ImGui::MenuItem("Save Model", "", &check))
            {
                SaveModelFile();
            }

            if (ImGui::MenuItem("Save Material", "", &check))
            {
                SaveMaterialFile();
            }

            if (ImGui::MenuItem("Save Node", "", &check))
            {
                SaveNodeFile();
            }

            if (ImGui::MenuItem("Save Animation", "", &check))
            {
                SaveAnimationFile();
            }

            ImGui::EndMenu();
        }

        //全部保存
        if (ImGui::BeginMenu("AllSave"))
        {
            bool check = false;
            if (ImGui::MenuItem("AllDataSave", "", &check))
            {
                SaveModelFile();
                SaveMaterialFile();
                SaveNodeFile();
                SaveAnimationFile();
            }
            ImGui::EndMenu();
        }

        // ギズモオペレーション
        if (ImGui::BeginMenu("Guizmo Operation"))
        {
            bool translate = guizmoOperation == ImGuizmo::TRANSLATE;
            if (ImGui::MenuItem("Translate", "", &translate))
            {
                guizmoOperation = ImGuizmo::TRANSLATE;
            }

            bool rotate = guizmoOperation == ImGuizmo::ROTATE;
            if (ImGui::MenuItem("Rotate", "", &rotate))
            {
                guizmoOperation = ImGuizmo::ROTATE;
            }

            bool scale = guizmoOperation == ImGuizmo::SCALE;
            if (ImGui::MenuItem("Scale", "", &scale))
            {
                guizmoOperation = ImGuizmo::SCALE;
            }

            ImGui::EndMenu();
        }
        // ギズモモード
        if (ImGui::BeginMenu("Guizmo Mode"))
        {
            bool local = guizmoMode == ImGuizmo::LOCAL;
            if (ImGui::MenuItem("Local", "", &local))
            {
                guizmoMode = ImGuizmo::LOCAL;
            }

            bool world = guizmoMode == ImGuizmo::WORLD;
            if (ImGui::MenuItem("World", "", &world))
            {
                guizmoMode = ImGuizmo::WORLD;
            }

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}
//
// ヒエラルキーGUI描画
void SceneTool::DrawNodeGUI(Model::Node* node)
{
    // 矢印をクリック、またはノードをダブルクリックで階層を開く
    ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

    // 選択フラグ
    if (selectionNode == node)
    {
        nodeFlags |= ImGuiTreeNodeFlags_Selected;
    }
    // 子がいない場合は矢印をつけない
    size_t child_count = node->children.size();
    if (child_count <= 0)
    {
        nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    }

    // ツリーノードを表示
    bool opened = ImGui::TreeNodeEx(node, nodeFlags, node->name);

    // アクティブ化されたノードは選択する
    if (ImGui::IsItemClicked())
    {
        selectionNode = node;
    }

    // 開かれている場合、子階層も同じ処理を行う
    if (opened && child_count > 0)
    {
        for (Model::Node* child : node->children)
        {
            DrawNodeGUI(child);
        }
        ImGui::TreePop();
    }
}

// ヒエラルキーGUI描画
void SceneTool::DrawHierarchyGUI()
{
    hiddenHierarchy = !ImGui::Begin("Hierarchy", nullptr, ImGuiWindowFlags_None);
    if (!hiddenHierarchy)
    {
        if (model != nullptr)
        {
            for (Model::Node& node : model->GetNodes())
            {
                if (node.parent == nullptr)
                {
                    DrawNodeGUI(&node);
                }
            }
        }
    }
    ImGui::End();
}

// プロパティGUI描画
void SceneTool::DrawPropertyGUI()
{
    hiddenProperty = !ImGui::Begin("Property", nullptr, ImGuiWindowFlags_None);
    if (!hiddenProperty)
    {
        // ノードプロパティ
        if (selectionNode != nullptr)
        {
            if (ImGui::CollapsingHeader("Node", ImGuiTreeNodeFlags_DefaultOpen))
            {
                DirectX::XMFLOAT3 position = selectionNode->translate;
                if (ImGui::InputFloat3("Position", &position.x, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
                {
                    selectionNode->translate = position;
                }

                DirectX::XMFLOAT3 rotation;
                TransformUtils::QuaternionToRollPitchYaw(selectionNode->rotate, rotation.x, rotation.y, rotation.z);
                rotation.x = DirectX::XMConvertToDegrees(rotation.x);
                rotation.y = DirectX::XMConvertToDegrees(rotation.y);
                rotation.z = DirectX::XMConvertToDegrees(rotation.z);
                if (ImGui::InputFloat3("Rotation", &rotation.x, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
                {
                    rotation.x = DirectX::XMConvertToRadians(rotation.x);
                    rotation.y = DirectX::XMConvertToRadians(rotation.y);
                    rotation.z = DirectX::XMConvertToRadians(rotation.z);
                    DirectX::XMVECTOR Rotation = DirectX::XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
                    DirectX::XMStoreFloat4(&selectionNode->rotate, Rotation);
                }

                DirectX::XMFLOAT3 scale = selectionNode->scale;
                if (ImGui::InputFloat3("Scale", &scale.x, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
                {
                    selectionNode->scale = scale;
                }
            }
        }

        // マテリアルプロパティ
        ModelResource::Material* material = GetSelectionMaterial();
        if (material != nullptr)
        {
            if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
            {
                // ラベル名が重複してしまうと起きる問題の対応
                ImGui::PushID(0);
                {
                    char name[256];
                    ::strncpy_s(name, sizeof(name), material->name.c_str(), sizeof(name));
                    if (ImGui::InputText("Name", name, sizeof(name), ImGuiInputTextFlags_EnterReturnsTrue))
                    {
                        material->name = name;
                    }
                }
                ImGui::PopID();

                char textureFile[256];

                const char* fileKinds[6] = { "Diffuse","Normal","Metallic","Roughness","Ao","Emissive" };

                for (int i = 0; i < 6; ++i)
                {
                    ::strncpy_s(textureFile, sizeof(textureFile), material->textureFilename[i].c_str(), sizeof(textureFile));

                    ImGui::PushID(&material->textureFilename[i]);
                    if (ImGui::Button("..."))
                    {
                        const char* filter = "Texture Files(*.DDS;*.png;*.tga;*.jpg;*.tif)\0*.DDS;*.png;*.tga;*.jpg;*.tif;\0All Files(*.*)\0*.*;\0\0";
                        DialogResult result = Dialog::OpenFileName(textureFile, sizeof(textureFile), filter, nullptr, Framework::Instance().GetHwnd());
                        if (result == DialogResult::OK)
                        {
                            char drive[32], dir[256], dirname[256];
                            ::_splitpath_s(modelFilePath.c_str(), drive, sizeof(drive), dir, sizeof(dir), nullptr, 0, nullptr, 0);
                            ::_makepath_s(dirname, sizeof(dirname), drive, dir, nullptr, nullptr);
                            dirname[strlen(dirname) - 1] = '\0';
                            char relativeTextureFile[MAX_PATH];
                            PathRelativePathToA(relativeTextureFile, dirname, FILE_ATTRIBUTE_DIRECTORY, textureFile, FILE_ATTRIBUTE_ARCHIVE);

                            // 読み込み
                            material->textureFilename[i] = relativeTextureFile;
                            material->LoadTexture(Graphics::Instance().GetDevice(), textureFile, i);
                        }
                    }
                    ImGui::SameLine();

                    ::strncpy_s(textureFile, sizeof(textureFile), material->textureFilename[i].c_str(), sizeof(textureFile));
                    if (ImGui::InputText(fileKinds[i], textureFile, sizeof(textureFile), ImGuiInputTextFlags_EnterReturnsTrue))
                    {
                        material->textureFilename[i] = textureFile;

                        char drive[32], dir[256], fullPath[256];
                        ::_splitpath_s(modelFilePath.c_str(), drive, sizeof(drive), dir, sizeof(dir), nullptr, 0, nullptr, 0);
                        ::_makepath_s(fullPath, sizeof(fullPath), drive, dir, textureFile, nullptr);
                        material->LoadTexture(Graphics::Instance().GetDevice(), fullPath, i);
                    }
                    ImGui::Text("TextureResource");
                    ImGui::Image(material->shaderResourceView[i].Get(), { 156, 156 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
                    ImGui::PopID();
                }
            }
        }

        // アニメーションプロパティ
        ModelResource::Animation* animation = GetSelectionAnimation();
        if (animation != nullptr)
        {
            if (ImGui::CollapsingHeader("Animation", ImGuiTreeNodeFlags_DefaultOpen))
            {
                // ラベル名が重複してしまうと起きる問題の対応
                ImGui::PushID(1);
                {
                    char name[256];
                    ::strncpy_s(name, sizeof(name), animation->name.c_str(), sizeof(name));
                    if (ImGui::InputText("Name", name, sizeof(name), ImGuiInputTextFlags_EnterReturnsTrue))
                    {
                        animation->name = name;
                    }
                }
                ImGui::PopID();
            }
        }
    }
    ImGui::End();
}
//
// アニメーションGUI描画
void SceneTool::DrawAnimationGUI()
{
    hiddenAnimation = !ImGui::Begin("Animation", nullptr, ImGuiWindowFlags_None);
    if (!hiddenAnimation)
    {
        if (modelResource != nullptr)
        {
            ModelResource::Animation* selectionAnimation = GetSelectionAnimation();

            int animationIndex = 0;

            std::vector<ModelResource::Animation>& animations = const_cast<std::vector<ModelResource::Animation>&>(modelResource->GetAnimations());
            std::vector<ModelResource::Animation>::iterator it = animations.begin();
            while (it != animations.end())
            {
                ModelResource::Animation& animation = (*it);

                ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_Leaf;
                if (selectionAnimation == &animation)
                {
                    nodeFlags |= ImGuiTreeNodeFlags_Selected;
                }

                ImGui::TreeNodeEx(&animation, nodeFlags, animation.name.c_str());

                // ドラッグアンドドロップによる要素の入れ替え
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
                {
                    ImGui::SetDragDropPayload("ANIMATIONS", &animationIndex, sizeof(int));
                    ImGui::Text("Dragging Animation : %s", animation.name.c_str());
                    ImGui::EndDragDropSource();
                }

                //アニメーション入れ替え
                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ANIMATIONS"))
                    {
                        if (payload->Data != nullptr)
                        {
                            int payloadIndex = *(const int*)payload->Data;
                            // 要素の入れ替えを行う
                            std::iter_swap(animations.begin() + animationIndex, animations.begin() + payloadIndex);
                            selectionAnimationIndex = animationIndex;
                        }
                    }
                    ImGui::EndDragDropTarget();
                }

                // コンテキストメニュー
                if (ImGui::BeginPopupContextItem())
                {
                    bool remove = ImGui::Selectable("Remove");

                    ImGui::EndPopup();

                    if (remove)
                    {
                        // アニメーションを停止する
                        model->StopAnimation();
                        selectionAnimationIndex = -1;
                        selectionNode = nullptr;

                        // アニメーション削除
                        it = animations.erase(it);
                        ImGui::TreePop();
                        ++animationIndex;
                        continue;
                    }
                }

                // クリックすると選択
                if (ImGui::IsItemClicked())
                {
                    selectionAnimationIndex = animationIndex;
                    selectionNode = nullptr;
                }

                ImGui::TreePop();
                ++animationIndex;
                ++it;
            }
        }
    }
    ImGui::End();
}

// タイムラインGUI描画
void SceneTool::DrawTimelineGUI()
{
    hiddenTimeline = !ImGui::Begin("Timeline", nullptr, ImGuiWindowFlags_None);
    if (!hiddenTimeline)
    {
        ModelResource::Animation* selectionAnimation = GetSelectionAnimation();
        if (model != nullptr && selectionAnimation != nullptr)
        {
            ImGui::Checkbox("Loop", &animationLoop);
            ImGui::SameLine();

            char name[256];
            ::strcpy_s(name, sizeof(name), selectionAnimation->name.c_str());
            ImGui::SetNextItemWidth(200);
            if (ImGui::InputText("Name", name, ImGuiInputTextFlags_EnterReturnsTrue))
            {
                selectionAnimation->name = name;
            }

            ImGui::DragFloat("AnimationSpeed", &selectionAnimation->animationspeed, 0.1f, 0.0f, 5.0f);

            if (ImGui::Button("Play"))
            {
                model->PlayAnimation(selectionAnimationIndex, animationLoop);
                model->SetSpeed(1.0f);
            }
            ImGui::SameLine();

            float animationCurrentSeconds = model->GetAnimationSeconds();
            int animationCurrentFrame = static_cast<int>(animationCurrentSeconds * 60.0f);
            int frameLength = static_cast<int>(selectionAnimation->secondsLength * 60);
            ImGui::SetNextItemWidth(50);
            if (ImGui::DragInt("", &animationCurrentFrame, 1.0f, 0, frameLength))
            {
                model->PlayAnimation(selectionAnimationIndex, animationLoop);
                model->SetAnimationSeconds(animationCurrentFrame / 60.0f);
                model->SetSpeed(0.0f);
            }
            ImGui::SameLine();

            if (ImGui::SliderFloat("Timeline", &animationCurrentSeconds, 0, selectionAnimation->secondsLength, "current frame = %.3f"))
            {
                model->PlayAnimation(selectionAnimationIndex, animationLoop);
                model->SetAnimationSeconds(animationCurrentSeconds);
                model->SetSpeed(0.0f);
            }

            std::vector<ModelResource::AnimationEvent>& animationevents = selectionAnimation->animationevents;
            if (ImGui::CollapsingHeader("AnimationEvents", ImGuiTreeNodeFlags_DefaultOpen))
            {
                if (selectionAnimationevent >= 0)
                {
                    ImGui::SameLine();
                    ImGui::TextUnformatted(animationevents[selectionAnimationevent].name.c_str());
                }

                if (ImGui::BeginPopup("my_select_popup"))
                {
                    for (int i = 0; i < animationevents.size(); i++)
                    {
                        if (ImGui::Selectable(animationevents[i].name.c_str()))
                        {
                            selectionAnimationevent = i;
                        }
                    }
                    ImGui::EndPopup();
                }
                ImGui::Separator();

                int animationeventcount = animationevents.size();
                ImGui::InputInt("EventCount", &animationeventcount, 1, 100, ImGuiInputTextFlags_ReadOnly);

                //アニメーションイベントを選択
                if (ImGui::Button("AnimationEventSelect"))
                {
                    ImGui::OpenPopup("my_select_popup");
                }

                //アニメーションイベント追加
                if (ImGui::Button("AddAnimationEvent"))
                {
                    ModelResource::AnimationEvent ae;
                    ae.name = std::to_string(animationevents.size());
                    ae.startframe = 0;
                    ae.endframe = 0;
                    animationevents.emplace_back(ae);
                }
                ImGui::SameLine();

                //アニメーションイベント削除
                if (ImGui::Button("Delete"))
                {
                    if (selectionAnimationevent >= 0)
                    {
                        animationevents.erase(animationevents.begin() + selectionAnimationevent);
                    }
                    if (selectionAnimationevent >= animationevents.size())
                    {
                        selectionAnimationevent = animationevents.size() - 1;
                    }
                }
                ImGui::Separator();
            }

            if (ImGui::CollapsingHeader("SelectEvent"))
            {
                if (selectionAnimationevent >= 0)
                {
                    //詳細表示
                    ModelResource::AnimationEvent& event = animationevents[selectionAnimationevent];

                    ImGui::PushID(2);
                    {
                        char name[256];
                        ::strncpy_s(name, sizeof(name), event.name.c_str(), sizeof(name));
                        if (ImGui::InputText("EventName", name, sizeof(name), ImGuiInputTextFlags_EnterReturnsTrue))
                        {
                            if (strlen(name) > 0)
                            {
                                event.name = name;
                            }
                        }
                    }
                    ImGui::PopID();

                    ImGui::Separator();

                    //フレーム登録
                    ImGui::SetNextItemWidth(80);
                    float SF = event.startframe;
                    if (ImGui::InputFloat("StartFlame", &SF, 0, 0, nullptr))
                    {
                        event.startframe = SF;
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("AddStart"))
                    {
                        event.startframe = model->GetAnimationSeconds();
                    }

                    ImGui::SameLine();

                    ImGui::SetNextItemWidth(80);
                    float EF = event.endframe;
                    if (ImGui::InputFloat("EndFlame", &EF, 0, 0, nullptr))
                    {
                        event.endframe = EF;
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("AddEnd"))
                    {
                        event.endframe = model->GetAnimationSeconds();
                    }
                }
            }

            if (ImGui::CollapsingHeader("RootKey"))
            {
                std::vector<ModelResource::RootPosition>& rootP = selectionAnimation->rootpositions;

                float animationCurrentSeconds = model->GetAnimationSeconds();
                ImGui::Checkbox("RootGizmo", &rootGizmo);
                ImGui::SameLine();
                rootEditing |= ImGui::DragFloat3("RootPosition", &rootposition.x);

                //キー追加
                if (ImGui::Button("AddRootKey"))
                {
                    ModelResource::RootPosition rAdd;
                    rAdd.frame = animationCurrentSeconds;
                    rAdd.position = rootposition;

                    int index = 0;
                    for (ModelResource::RootPosition r : rootP)
                    {
                        if (r.frame > rAdd.frame)
                            break;
                        index++;
                    }
                    if (index >= rootP.size())
                        rootP.emplace_back(rAdd);
                    else
                        rootP.insert(rootP.begin() + index, rAdd);
                }

                ImGui::SameLine();
                if (ImGui::Button("BeforeKey"))
                {
                    float nextFrame = -1;
                    for (int index = rootP.size() - 1; index >= 0; --index)
                    {
                        if (animationCurrentSeconds > rootP[index].frame)
                        {
                            nextFrame = rootP[index].frame;
                            break;
                        }
                    }

                    if (nextFrame >= 0)
                        model->SetAnimationSeconds(nextFrame);
                }
                ImGui::SameLine();
                if (ImGui::Button("NextKey"))
                {
                    float nextFrame = -1;
                    for (ModelResource::RootPosition r : rootP)
                    {
                        if (animationCurrentSeconds < r.frame)
                        {
                            nextFrame = r.frame;
                            break;
                        }
                    }

                    if (nextFrame >= 0)
                        model->SetAnimationSeconds(nextFrame);
                }

                //表示
                int deleteIndex = 0;
                for (ModelResource::RootPosition r : rootP)
                {
                    ImGui::SetNextItemWidth(50);
                    ImGui::DragFloat("Frame", &r.frame);
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(300);
                    ImGui::DragFloat3("Pos", &r.position.x);
                    ImGui::SameLine();
                    //削除
                    std::string d = "Delete" + std::to_string(deleteIndex);
                    if (ImGui::Button(d.c_str()))
                    {
                        rootP.erase(rootP.begin() + deleteIndex);
                    }
                    deleteIndex++;
                }

                //位置編集
                if (rootEditing)
                {
                    for (ModelResource::RootPosition& r : rootP)
                    {
                        int rF = r.frame * 1000;
                        int nF = animationCurrentSeconds * 1000;
                        if (rF != nF)continue;
                        r.position = rootposition;
                    }
                }
            }
        }
    }
    ImGui::End();
}

// マテリアルGUI描画
void SceneTool::DrawMaterialGUI()
{
    hiddenProperty = !ImGui::Begin("Material", nullptr, ImGuiWindowFlags_None);
    if (!hiddenProperty)
    {
        if (modelResource != nullptr)
        {
            //デバッグ用にシェーダー定数文字列のリスト生成
            constexpr const char* ModelShaderNames[] =
            {
                "Defalt",
                "Unity",
            };
            //	シェーダー定数とシェーダー名リストとのサイズが違うとエラーを出す
            static_assert(ARRAYSIZE(ModelShaderNames) != static_cast<int>(MODELSHADER::SHADERMAX) - 1, "ModelShaderNames Size Error!");
            //シェーダー
            ImGui::Combo("Shader", &shaderid, ModelShaderNames, static_cast<int>(MODELSHADER::SHADERMAX), 10);

            int materialIndex = 0;
            for (const ModelResource::Material& material : modelResource->GetMaterials())
            {
                ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_Leaf;

                if (selectionMaterialIndex == materialIndex)
                {
                    nodeFlags |= ImGuiTreeNodeFlags_Selected;
                }

                ImGui::TreeNodeEx(&material, nodeFlags, material.name.c_str());

                if (ImGui::IsItemClicked())
                {
                    selectionMaterialIndex = materialIndex;
                    selectionNode = nullptr;
                }

                ImGui::TreePop();

                ++materialIndex;
            }

            // マテリアルプロパティ
            ModelResource::Material* material = GetSelectionMaterial();
            if (material != nullptr)
            {
                //マテリアルのimgui
                ImGui::ColorEdit4("color", &material->color.x, ImGuiColorEditFlags_None);
                ImGui::ColorEdit3("emissioncolor", &material->emissivecolor.x, ImGuiColorEditFlags_None);
                ImGui::DragFloat("emissionintensity", &material->emissiveintensity, 1.0f, 0.0f, +500.0f);
                ImGui::DragFloat("roughness", &material->Roughness, 0.1f, 0.0f, 1.0f);
                ImGui::DragFloat("metalness", &material->Metalness, 0.1f, 0.0f, 1.0f);
            }

            if (selectionNode != nullptr)
            {
                int layer = selectionNode->layer;
                if (ImGui::InputInt("Layer", &layer))
                {
                    selectionNode->layer = layer;
                    if (nodeselect)
                    {
                        for (Model::Node* child : selectionNode->children)
                        {
                            child->layer = layer;
                        }
                    }
                }
                ImGui::Checkbox("NodeSelect", &nodeselect);
            }
        }
    }
    ImGui::End();
}

// モデルファイルを開く
void SceneTool::OpenModelFile()
{
    //ImportModel("Data/Model/OneCoin/robot.mdl");

    static const char* filter = "Model Files(*.fbx;*.mdl)\0*.fbx;*.mdl;\0All Files(*.*)\0*.*;\0\0";

    char filename[256] = { 0 };
    DialogResult result = Dialog::OpenFileName(filename, sizeof(filename), filter, nullptr, Framework::Instance().GetHwnd());
    if (result == DialogResult::OK)
    {
        ImportModel(filename);
    }
}

// モデルファイルを保存
void SceneTool::SaveModelFile()
{
    static const char* filter = "Model Files(*.mdl)\0*.mdl;\0All Files(*.*)\0*.*;\0\0";

    char filename[256] = { 0 };
    DialogResult result = Dialog::SaveFileName(filename, sizeof(filename), filter, nullptr, "mdl", Framework::Instance().GetHwnd());
    if (result == DialogResult::OK)
    {
        ExportModel(filename);
    }
}

// モデル入力
void SceneTool::ImportModel(const char* filename)
{
    modelFilePath = filename;

    ID3D11Device* device = Graphics::Instance().GetDevice();
    modelResource = std::make_shared<FbxModelResource>();
    modelResource->Load(device, filename);
    model = std::make_unique<Model>(modelResource);

    selectionNode = nullptr;
    selectionAnimationIndex = -1;
}

// モデル出力
void SceneTool::ExportModel(const char* filename)
{
    if (modelResource != nullptr)
    {
        modelResource->Export(filename);
    }
}

//マテリアルファイルを開く
void SceneTool::OpenMaterial()
{
    static const char* filter = "Material Files(*.Material)\0*.Material;\0All Files(*.*)\0*.*;\0\0";

    char filename[256] = { 0 };
    DialogResult result = Dialog::OpenFileName(filename, sizeof(filename), filter, nullptr, Framework::Instance().GetHwnd());
    if (result == DialogResult::OK)
    {
        //マテリアル入力
        ImportMaterial(filename);
    }
}

//マテリアルファイルを保存
void SceneTool::SaveMaterialFile()
{
    static const char* filter = "Material Files(*.Material)\0*.Material;\0All Files(*.*)\0*.*;\0\0";

    char filename[256] = { 0 };
    DialogResult result = Dialog::SaveFileName(filename, sizeof(filename), filter, nullptr, "Material", Framework::Instance().GetHwnd());
    if (result == DialogResult::OK)
    {
        ExportMaterial(filename);
    }
}

//マテリアル入力
void SceneTool::ImportMaterial(const char* filename)
{
    modelFilePath = filename;

    ID3D11Device* device = Graphics::Instance().GetDevice();
    modelResource->LoadMaterial(device, filename);

    selectionNode = nullptr;
    selectionAnimationIndex = -1;
}

//マテリアル出力
void SceneTool::ExportMaterial(const char* filename)
{
    if (modelResource != nullptr)
    {
        modelResource->ExportMaterial(filename);
    }
}

// アニメーションファイルを開く
void SceneTool::OpenAnimationFile()
{
    static const char* filter = "Animation Files(*.fbx)\0*.fbx;\0All Files(*.*)\0*.*;\0\0";

    int size = 256 * 100;
    std::unique_ptr<char[]> buffer = std::make_unique<char[]>(size);
    ::memset(buffer.get(), 0, size);
    DialogResult result = Dialog::OpenFileName(buffer.get(), size, filter, nullptr, Framework::Instance().GetHwnd(), true);
    if (result == DialogResult::OK)
    {
        // 複数ファイル選択時はNULLを区切り文字として複数の文字列が格納されている。
        // 始めの文字列がディレクトリ名で以降がファイル名が格納されている。
        // 終端はNULLが連続で格納されている。
        // ※単一ファイル選択時はフルパスが格納されている。
        const char* dir = buffer.get();
        const char* fname = dir + strlen(dir) + 1;
        if (fname[0] == '\0')
        {
            fname = dir;
            dir = "";
        }
        while (fname[0] != '\0')
        {
            // ファイルパス作成
            char filename[256];
            ::_makepath_s(filename, nullptr, dir, fname, nullptr);

            // アニメーションインポート
            ImportAnimation(filename);

            // 次のファイル
            fname = fname + strlen(fname) + 1;
        }
    }
}

// アニメーション入力
void SceneTool::ImportAnimation(const char* filename)
{
    if (modelResource != nullptr)
    {
        modelResource->AddAnimation(filename);
    }
}

//既存のアニメーションファイルを保存
void SceneTool::SaveAnimationFile()
{
    static const char* filter = "Animation Files(*.Animation)\0*.Animation;\0All Files(*.*)\0*.*;\0\0";

    char filename[256] = { 0 };
    DialogResult result = Dialog::SaveFileName(filename, sizeof(filename), filter, nullptr, "Animation", Framework::Instance().GetHwnd());
    if (result == DialogResult::OK)
    {
        ExportAnimation(filename);
    }
}

//ノードを保存
void SceneTool::SaveNodeFile()
{
    static const char* filter = "Node Files(*.Node)\0*.Node;\0All Files(*.*)\0*.*;\0\0";

    char filename[256] = { 0 };
    DialogResult result = Dialog::SaveFileName(filename, sizeof(filename), filter, nullptr, "Node", Framework::Instance().GetHwnd());
    if (result == DialogResult::OK)
    {
        ExportNode(filename);
    }
}

//既存のアニメーションファイルを開く
void SceneTool::ExisitingOpenAnimationFile()
{
    static const char* filter = "Animation Files(*.Animation)\0*.Animation;\0All Files(*.*)\0*.*;\0\0";

    char filename[256] = { 0 };
    DialogResult result = Dialog::OpenFileName(filename, sizeof(filename), filter, nullptr, Framework::Instance().GetHwnd());
    if (result == DialogResult::OK)
    {
        //アニメーション入力
        ImportExisitingAnimation(filename);
    }
}

//ノードファイルを開く
void SceneTool::OpenNodeFile()
{
    static const char* filter = "Node Files(*.Node)\0*.Node;\0All Files(*.*)\0*.*;\0\0";

    char filename[256] = { 0 };
    DialogResult result = Dialog::OpenFileName(filename, sizeof(filename), filter, nullptr, Framework::Instance().GetHwnd());
    if (result == DialogResult::OK)
    {
        //ノード入力
        ImportNode(filename);
    }
}

//既存のアニメーション入力
void SceneTool::ImportExisitingAnimation(const char* filename)
{
    modelFilePath = filename;

    ID3D11Device* device = Graphics::Instance().GetDevice();
    modelResource->LoadAnimation(device, filename);

    selectionNode = nullptr;
    selectionAnimationIndex = -1;
}

//ノード入力
void SceneTool::ImportNode(const char* filename)
{
    modelFilePath = filename;

    ID3D11Device* device = Graphics::Instance().GetDevice();
    modelResource->LoadNode(device, filename);
    model->CopyRefrectModel();

    selectionNode = nullptr;
    selectionAnimationIndex = -1;
}

//既存のアニメーション出力
void SceneTool::ExportAnimation(const char* filename)
{
    if (modelResource != nullptr)
    {
        modelResource->ExportAnimation(filename);
    }
}

//ノード出力
void SceneTool::ExportNode(const char* filename)
{
    if (modelResource != nullptr)
    {
        modelResource->ExportNode(filename);
    }
}

//ルートモーション更新
void SceneTool::UpdateRootMotion()
{
    ModelResource::Animation* selectionAnimation = GetSelectionAnimation();
    if (model != nullptr && selectionAnimation != nullptr)
    {
        std::vector<ModelResource::RootPosition>& rootP = selectionAnimation->rootpositions;
        float animationCurrentSeconds = model->GetAnimationSeconds();

        //位置合わせ
        for (ModelResource::RootPosition& r : rootP)
        {
            int rF = r.frame * 1000;
            int nF = animationCurrentSeconds * 1000;
            if (rF != nF)continue;
            rootposition = r.position;
        }

        //ルートの移動
        int keyCount = rootP.size();
        for (int keyIndex = 0; keyIndex < keyCount - 1; ++keyIndex)
        {
            // 現在の時間がどのキーフレームの間にいるか判定する
            float frame0 = rootP[keyIndex].frame;
            float frame1 = rootP[keyIndex + 1].frame;
            if (animationCurrentSeconds >= frame0 && animationCurrentSeconds <= frame1)
            {
                float rate = (animationCurrentSeconds - frame0) / (frame1 - frame0);
                // ２つのキーフレーム間の補完計算
                DirectX::XMVECTOR T0 = DirectX::XMLoadFloat3(&rootP[keyIndex].position);
                DirectX::XMVECTOR T1 = DirectX::XMLoadFloat3(&rootP[keyIndex + 1].position);
                DirectX::XMVECTOR T = DirectX::XMVectorLerp(T0, T1, rate);

                DirectX::XMStoreFloat3(&rootposition, T);
                break;
            }
        }
    }
}

// 選択アニメーションを取得
ModelResource::Animation* SceneTool::GetSelectionAnimation()
{
    if (modelResource != nullptr)
    {
        const std::vector<ModelResource::Animation>& animations = modelResource->GetAnimations();
        if (selectionAnimationIndex >= 0 && selectionAnimationIndex < static_cast<int>(animations.size()))
        {
            return const_cast<ModelResource::Animation*>(&animations.at(selectionAnimationIndex));
        }
    }
    return nullptr;
}

// 選択マテリアルを取得
ModelResource::Material* SceneTool::GetSelectionMaterial()
{
    if (modelResource != nullptr)
    {
        const std::vector<ModelResource::Material>& materials = modelResource->GetMaterials();
        if (selectionMaterialIndex >= 0 && selectionMaterialIndex < static_cast<int>(materials.size()))
        {
            return const_cast<ModelResource::Material*>(&materials.at(selectionMaterialIndex));
        }
    }
    return nullptr;
}

////コリジョンGUI描画
//void Framework::DrawCollsionGUI()
//{
//    ImGui::Begin("Collsion");
//    if (selectionCollsionNode != nullptr)
//    {
//        std::vector<ModelResource::Collsion>& collisions = model->GetResource()->GetCollsions();
//
//        char name[256];
//        strcpy(name, selectionCollsionNode->name);
//        ImGui::InputText("CollsionNodeName", name, sizeof(name));
//
//        //追加
//        if (ImGui::Button("AddCollsion"))
//        {
//            ModelResource::Collsion col;
//            col.name = name;
//            collisions.emplace_back(col);
//        }
//
//        ImGui::SameLine();
//
//        //削除
//        if (ImGui::Button("Delete"))
//        {
//            if (selectioncollsionindex >= 0)
//            {
//                collisions.erase(collisions.begin() + selectioncollsionindex);
//            }
//            if (selectioncollsionindex >= collisions.size())
//            {
//                selectioncollsionindex = collisions.size() - 1;
//            }
//        }
//
//        ImGui::Separator();
//
//        if (ImGui::CollapsingHeader("CollsionList", ImGuiTreeNodeFlags_DefaultOpen))
//        {
//            for (int i = 0; i < collisions.size(); i++)
//            {
//                bool select = (selectioncollsionindex == i);
//                std::string collisionName = collisions[i].name + " " + std::to_string(i);
//                if (ImGui::Selectable(collisionName.c_str(), &select))
//                {
//                    selectioncollsionindex = i;
//                }
//            }
//        }
//
//        if (selectioncollsionindex >= 0 && selectioncollsionindex < collisions.size())
//        {
//            ModelResource::Collsion* col = &collisions[selectioncollsionindex];
//            ImGui::InputFloat3("Pos", &col->position.x);
//        }
//    }
//    ImGui::End();
//}