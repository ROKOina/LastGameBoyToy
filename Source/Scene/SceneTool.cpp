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

//������
void SceneTool::Initialize()
{
    //�|�X�g�G�t�F�N�g����
    posteffect = std::make_unique<PostEffect>();

    //�R���X�^���g�o�b�t�@�̏�����
    ConstantBufferInitialize();

    //���s������ǉ�
    Light* mainDirectionalLight = new Light(LightType::Directional);
    mainDirectionalLight->SetDirection({ -0.5f, -0.5f, 0 });
    mainDirectionalLight->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
    LightManager::Instance().Register(mainDirectionalLight);
}

//�X�V����
void SceneTool::Update(float elapsedTime)
{
    //�G�鎞�ɓ����Ă��܂��̂�΍􂷂��
    rootEditing = false;

    // ���f���X�V
    if (model != nullptr)
    {
        model->UpdateAnimation(elapsedTime);
        model->UpdateTransform(DirectX::XMMatrixIdentity());
        model->UpdateTransform(DirectX::XMMatrixTranslation(rootposition.x, rootposition.y, rootposition.z));
        UpdateRootMotion();
        model->CopyModel();
    }

    //�J�����X�V
    camera.Update();
}

//�`�揈��
void SceneTool::Render()
{
    //viewport�̐ݒ�
    ViewPortInitialize();

    //�R���X�^���g�o�b�t�@�̍X�V
    ConstantBufferUpdate();

    //�|�X�g�G�t�F�N�g�̑O����
    posteffect->BeforeSet();

    // ���C�g�̒萔�o�b�t�@���X�V
    LightManager::Instance().UpdateConstatBuffer();

    //���f���̕`��
    if (model != nullptr)
    {
        Graphics::Instance().GetModelShader(shaderid)->Render(Graphics::Instance().GetDeviceContext(), model.get());
    }

    //Debug�`��
    DebugRender();

    //�|�X�g�G�t�F�N�g�̌㏈��
    posteffect->AfterSet();

    // GUI�`��
    DrawGUI();
}

// �M�Y���`��
void SceneTool::DrawGuizmo(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection, Model::Node* node)
{
    // �I���m�[�h�̍s��𑀍삷��
    ImGuizmo::Manipulate(
        &view._11, &projection._11,	// �r���[���v���W�F�N�V�����s��
        guizmoOperation,			// ����
        guizmoMode,					// ���
        &node->worldTransform._11,	// ���삷�郏�[���h�s��
        nullptr);

    // ���삵�����[���h�s������[�J���s��ɔ��f
    if (node->parent != nullptr)
    {
        // �e�̃��[���h�t�s��Ǝ��g�̃��[���h�s�����Z���邱�ƂŎ��g�̃��[�J���s����Z�o�ł���
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

    // ���[�J���s�񂩂�SRT�l�ɔ��f����
    TransformUtils::MatrixToTransformation(node->localTransform, &node->scale, &node->rotate, &node->translate);
}

// ���[�g���[�V�����p�M�Y���`��
void SceneTool::DrawRootGuizmo(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection, DirectX::XMFLOAT3& rootPos)
{
    DirectX::XMFLOAT4X4 rootTransform = {
   1,0,0,0,
   0,1,0,0,
   0,0,1,0,
   rootPos.x,rootPos.y,rootPos.z,1,
    };

    // �I���m�[�h�̍s��𑀍삷��
    rootEditing |= ImGuizmo::Manipulate(
        &view._11, &projection._11,    // �r���[���v���W�F�N�V�����s��
        guizmoOperation,            // ����
        guizmoMode,                    // ���
        &rootTransform._11,    // ���삷�郏�[���h�s��
        nullptr);

    // ���[�J���s�񂩂�SRT�l�ɔ��f����
    TransformUtils::MatrixToTransformation(rootTransform, nullptr, nullptr, &rootPos);
}

//�O���b�h�`��
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

    // X��
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

    // Y��
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

    // Z��
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

//Debug�`��
void SceneTool::DebugRender()
{
    Graphics& Graphics = Graphics::Instance();
    ID3D11DeviceContext* dc = Graphics.GetDeviceContext();

    //�M�Y���̈ʒu����
    ImGuizmo::BeginFrame();
    ImGuizmo::SetOrthographic(false);
    RECT r;
    ::GetWindowRect(Framework::Instance().GetHwnd(), &r);
    ImGuizmo::SetRect(r.left, r.top, r.right - r.left, r.bottom - r.top);

    // �M�Y���`��
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

    // �O���b�h�`��
    DrawGrid(dc, 20, 10.0f);

    //���C���`��
    Graphics.GetLineRenderer()->Render(dc, sc->data.view, sc->data.projection);
}

// GUI�`��
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

// ���j���[GUI�`��
void SceneTool::DrawMenuGUI()
{
    if (ImGui::BeginMainMenuBar())
    {
        // �t�@�C�����j���[
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

        //�Z�[�u(��)
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

        //�S���ۑ�
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

        // �M�Y���I�y���[�V����
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
        // �M�Y�����[�h
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
// �q�G�����L�[GUI�`��
void SceneTool::DrawNodeGUI(Model::Node* node)
{
    // �����N���b�N�A�܂��̓m�[�h���_�u���N���b�N�ŊK�w���J��
    ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

    // �I���t���O
    if (selectionNode == node)
    {
        nodeFlags |= ImGuiTreeNodeFlags_Selected;
    }
    // �q�����Ȃ��ꍇ�͖������Ȃ�
    size_t child_count = node->children.size();
    if (child_count <= 0)
    {
        nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    }

    // �c���[�m�[�h��\��
    bool opened = ImGui::TreeNodeEx(node, nodeFlags, node->name);

    // �A�N�e�B�u�����ꂽ�m�[�h�͑I������
    if (ImGui::IsItemClicked())
    {
        selectionNode = node;
    }

    // �J����Ă���ꍇ�A�q�K�w�������������s��
    if (opened && child_count > 0)
    {
        for (Model::Node* child : node->children)
        {
            DrawNodeGUI(child);
        }
        ImGui::TreePop();
    }
}

// �q�G�����L�[GUI�`��
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

// �v���p�e�BGUI�`��
void SceneTool::DrawPropertyGUI()
{
    hiddenProperty = !ImGui::Begin("Property", nullptr, ImGuiWindowFlags_None);
    if (!hiddenProperty)
    {
        // �m�[�h�v���p�e�B
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

        // �}�e���A���v���p�e�B
        ModelResource::Material* material = GetSelectionMaterial();
        if (material != nullptr)
        {
            if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
            {
                // ���x�������d�����Ă��܂��ƋN������̑Ή�
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

                            // �ǂݍ���
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

        // �A�j���[�V�����v���p�e�B
        ModelResource::Animation* animation = GetSelectionAnimation();
        if (animation != nullptr)
        {
            if (ImGui::CollapsingHeader("Animation", ImGuiTreeNodeFlags_DefaultOpen))
            {
                // ���x�������d�����Ă��܂��ƋN������̑Ή�
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
// �A�j���[�V����GUI�`��
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

                // �h���b�O�A���h�h���b�v�ɂ��v�f�̓���ւ�
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
                {
                    ImGui::SetDragDropPayload("ANIMATIONS", &animationIndex, sizeof(int));
                    ImGui::Text("Dragging Animation : %s", animation.name.c_str());
                    ImGui::EndDragDropSource();
                }

                //�A�j���[�V��������ւ�
                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ANIMATIONS"))
                    {
                        if (payload->Data != nullptr)
                        {
                            int payloadIndex = *(const int*)payload->Data;
                            // �v�f�̓���ւ����s��
                            std::iter_swap(animations.begin() + animationIndex, animations.begin() + payloadIndex);
                            selectionAnimationIndex = animationIndex;
                        }
                    }
                    ImGui::EndDragDropTarget();
                }

                // �R���e�L�X�g���j���[
                if (ImGui::BeginPopupContextItem())
                {
                    bool remove = ImGui::Selectable("Remove");

                    ImGui::EndPopup();

                    if (remove)
                    {
                        // �A�j���[�V�������~����
                        model->StopAnimation();
                        selectionAnimationIndex = -1;
                        selectionNode = nullptr;

                        // �A�j���[�V�����폜
                        it = animations.erase(it);
                        ImGui::TreePop();
                        ++animationIndex;
                        continue;
                    }
                }

                // �N���b�N����ƑI��
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

// �^�C�����C��GUI�`��
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

                //�A�j���[�V�����C�x���g��I��
                if (ImGui::Button("AnimationEventSelect"))
                {
                    ImGui::OpenPopup("my_select_popup");
                }

                //�A�j���[�V�����C�x���g�ǉ�
                if (ImGui::Button("AddAnimationEvent"))
                {
                    ModelResource::AnimationEvent ae;
                    ae.name = std::to_string(animationevents.size());
                    ae.startframe = 0;
                    ae.endframe = 0;
                    animationevents.emplace_back(ae);
                }
                ImGui::SameLine();

                //�A�j���[�V�����C�x���g�폜
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
                    //�ڍו\��
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

                    //�t���[���o�^
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

                //�L�[�ǉ�
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

                //�\��
                int deleteIndex = 0;
                for (ModelResource::RootPosition r : rootP)
                {
                    ImGui::SetNextItemWidth(50);
                    ImGui::DragFloat("Frame", &r.frame);
                    ImGui::SameLine();
                    ImGui::SetNextItemWidth(300);
                    ImGui::DragFloat3("Pos", &r.position.x);
                    ImGui::SameLine();
                    //�폜
                    std::string d = "Delete" + std::to_string(deleteIndex);
                    if (ImGui::Button(d.c_str()))
                    {
                        rootP.erase(rootP.begin() + deleteIndex);
                    }
                    deleteIndex++;
                }

                //�ʒu�ҏW
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

// �}�e���A��GUI�`��
void SceneTool::DrawMaterialGUI()
{
    hiddenProperty = !ImGui::Begin("Material", nullptr, ImGuiWindowFlags_None);
    if (!hiddenProperty)
    {
        if (modelResource != nullptr)
        {
            //�f�o�b�O�p�ɃV�F�[�_�[�萔������̃��X�g����
            constexpr const char* ModelShaderNames[] =
            {
                "Defalt",
                "Unity",
            };
            //	�V�F�[�_�[�萔�ƃV�F�[�_�[�����X�g�Ƃ̃T�C�Y���Ⴄ�ƃG���[���o��
            static_assert(ARRAYSIZE(ModelShaderNames) != static_cast<int>(MODELSHADER::SHADERMAX) - 1, "ModelShaderNames Size Error!");
            //�V�F�[�_�[
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

            // �}�e���A���v���p�e�B
            ModelResource::Material* material = GetSelectionMaterial();
            if (material != nullptr)
            {
                //�}�e���A����imgui
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

// ���f���t�@�C�����J��
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

// ���f���t�@�C����ۑ�
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

// ���f������
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

// ���f���o��
void SceneTool::ExportModel(const char* filename)
{
    if (modelResource != nullptr)
    {
        modelResource->Export(filename);
    }
}

//�}�e���A���t�@�C�����J��
void SceneTool::OpenMaterial()
{
    static const char* filter = "Material Files(*.Material)\0*.Material;\0All Files(*.*)\0*.*;\0\0";

    char filename[256] = { 0 };
    DialogResult result = Dialog::OpenFileName(filename, sizeof(filename), filter, nullptr, Framework::Instance().GetHwnd());
    if (result == DialogResult::OK)
    {
        //�}�e���A������
        ImportMaterial(filename);
    }
}

//�}�e���A���t�@�C����ۑ�
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

//�}�e���A������
void SceneTool::ImportMaterial(const char* filename)
{
    modelFilePath = filename;

    ID3D11Device* device = Graphics::Instance().GetDevice();
    modelResource->LoadMaterial(device, filename);

    selectionNode = nullptr;
    selectionAnimationIndex = -1;
}

//�}�e���A���o��
void SceneTool::ExportMaterial(const char* filename)
{
    if (modelResource != nullptr)
    {
        modelResource->ExportMaterial(filename);
    }
}

// �A�j���[�V�����t�@�C�����J��
void SceneTool::OpenAnimationFile()
{
    static const char* filter = "Animation Files(*.fbx)\0*.fbx;\0All Files(*.*)\0*.*;\0\0";

    int size = 256 * 100;
    std::unique_ptr<char[]> buffer = std::make_unique<char[]>(size);
    ::memset(buffer.get(), 0, size);
    DialogResult result = Dialog::OpenFileName(buffer.get(), size, filter, nullptr, Framework::Instance().GetHwnd(), true);
    if (result == DialogResult::OK)
    {
        // �����t�@�C���I������NULL����؂蕶���Ƃ��ĕ����̕����񂪊i�[����Ă���B
        // �n�߂̕����񂪃f�B���N�g�����ňȍ~���t�@�C�������i�[����Ă���B
        // �I�[��NULL���A���Ŋi�[����Ă���B
        // ���P��t�@�C���I�����̓t���p�X���i�[����Ă���B
        const char* dir = buffer.get();
        const char* fname = dir + strlen(dir) + 1;
        if (fname[0] == '\0')
        {
            fname = dir;
            dir = "";
        }
        while (fname[0] != '\0')
        {
            // �t�@�C���p�X�쐬
            char filename[256];
            ::_makepath_s(filename, nullptr, dir, fname, nullptr);

            // �A�j���[�V�����C���|�[�g
            ImportAnimation(filename);

            // ���̃t�@�C��
            fname = fname + strlen(fname) + 1;
        }
    }
}

// �A�j���[�V��������
void SceneTool::ImportAnimation(const char* filename)
{
    if (modelResource != nullptr)
    {
        modelResource->AddAnimation(filename);
    }
}

//�����̃A�j���[�V�����t�@�C����ۑ�
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

//�m�[�h��ۑ�
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

//�����̃A�j���[�V�����t�@�C�����J��
void SceneTool::ExisitingOpenAnimationFile()
{
    static const char* filter = "Animation Files(*.Animation)\0*.Animation;\0All Files(*.*)\0*.*;\0\0";

    char filename[256] = { 0 };
    DialogResult result = Dialog::OpenFileName(filename, sizeof(filename), filter, nullptr, Framework::Instance().GetHwnd());
    if (result == DialogResult::OK)
    {
        //�A�j���[�V��������
        ImportExisitingAnimation(filename);
    }
}

//�m�[�h�t�@�C�����J��
void SceneTool::OpenNodeFile()
{
    static const char* filter = "Node Files(*.Node)\0*.Node;\0All Files(*.*)\0*.*;\0\0";

    char filename[256] = { 0 };
    DialogResult result = Dialog::OpenFileName(filename, sizeof(filename), filter, nullptr, Framework::Instance().GetHwnd());
    if (result == DialogResult::OK)
    {
        //�m�[�h����
        ImportNode(filename);
    }
}

//�����̃A�j���[�V��������
void SceneTool::ImportExisitingAnimation(const char* filename)
{
    modelFilePath = filename;

    ID3D11Device* device = Graphics::Instance().GetDevice();
    modelResource->LoadAnimation(device, filename);

    selectionNode = nullptr;
    selectionAnimationIndex = -1;
}

//�m�[�h����
void SceneTool::ImportNode(const char* filename)
{
    modelFilePath = filename;

    ID3D11Device* device = Graphics::Instance().GetDevice();
    modelResource->LoadNode(device, filename);
    model->CopyRefrectModel();

    selectionNode = nullptr;
    selectionAnimationIndex = -1;
}

//�����̃A�j���[�V�����o��
void SceneTool::ExportAnimation(const char* filename)
{
    if (modelResource != nullptr)
    {
        modelResource->ExportAnimation(filename);
    }
}

//�m�[�h�o��
void SceneTool::ExportNode(const char* filename)
{
    if (modelResource != nullptr)
    {
        modelResource->ExportNode(filename);
    }
}

//���[�g���[�V�����X�V
void SceneTool::UpdateRootMotion()
{
    ModelResource::Animation* selectionAnimation = GetSelectionAnimation();
    if (model != nullptr && selectionAnimation != nullptr)
    {
        std::vector<ModelResource::RootPosition>& rootP = selectionAnimation->rootpositions;
        float animationCurrentSeconds = model->GetAnimationSeconds();

        //�ʒu���킹
        for (ModelResource::RootPosition& r : rootP)
        {
            int rF = r.frame * 1000;
            int nF = animationCurrentSeconds * 1000;
            if (rF != nF)continue;
            rootposition = r.position;
        }

        //���[�g�̈ړ�
        int keyCount = rootP.size();
        for (int keyIndex = 0; keyIndex < keyCount - 1; ++keyIndex)
        {
            // ���݂̎��Ԃ��ǂ̃L�[�t���[���̊Ԃɂ��邩���肷��
            float frame0 = rootP[keyIndex].frame;
            float frame1 = rootP[keyIndex + 1].frame;
            if (animationCurrentSeconds >= frame0 && animationCurrentSeconds <= frame1)
            {
                float rate = (animationCurrentSeconds - frame0) / (frame1 - frame0);
                // �Q�̃L�[�t���[���Ԃ̕⊮�v�Z
                DirectX::XMVECTOR T0 = DirectX::XMLoadFloat3(&rootP[keyIndex].position);
                DirectX::XMVECTOR T1 = DirectX::XMLoadFloat3(&rootP[keyIndex + 1].position);
                DirectX::XMVECTOR T = DirectX::XMVectorLerp(T0, T1, rate);

                DirectX::XMStoreFloat3(&rootposition, T);
                break;
            }
        }
    }
}

// �I���A�j���[�V�������擾
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

// �I���}�e���A�����擾
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

////�R���W����GUI�`��
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
//        //�ǉ�
//        if (ImGui::Button("AddCollsion"))
//        {
//            ModelResource::Collsion col;
//            col.name = name;
//            collisions.emplace_back(col);
//        }
//
//        ImGui::SameLine();
//
//        //�폜
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